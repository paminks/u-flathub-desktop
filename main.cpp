/*
uFlathub-Desktop is an unofficial desktop client for flathub.org.
Copyright (C) 2023  Eren Aydın

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "header.h"


//shows if the given command executes or not
//not really necessary but yeah 
void runCommand(const char* command) {
    int result = system(command);
    if (result == 0) {
        g_print("Command executed successfully: %s\n", command);
    } else {
        g_print("Command execution failed: %s\n", command);
    }
}

static void decide_policy_cb(WebKitWebView* webView, WebKitPolicyDecision* decision, WebKitPolicyDecisionType type, gpointer user_data)
{
    //any comments about this is really welcome cus i dont know what is a webkit_navigation_policy_decision_get_navigation_action
    //but i know this prints outs the every link user clicks
    if (type == WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION) {
        WebKitNavigationAction* navigationAction = webkit_navigation_policy_decision_get_navigation_action(WEBKIT_NAVIGATION_POLICY_DECISION(decision));
        WebKitURIRequest* uriRequest = webkit_navigation_action_get_request(navigationAction);
        const gchar* uri = webkit_uri_request_get_uri(uriRequest);

        g_print("%s\n",uri);

        /*checks if the clicked url starts with given link and i think if you are not downloading from Germany mirror you should change 
        https://dl.flathub.org/repo/appstream to something like https://us.flathub.org/repo/appstream or https://nl.flathub.org/repo/appstream i dont know.
        check your mirror by downloading a flatpak from flathub.org and looking it up on download section of your browser
        */
        const gchar* prefix = "https://dl.flathub.org/repo/appstream/";
        if (g_str_has_prefix(uri, prefix)) {
            // Create an output file path
            gchar* filename = g_strdup_printf("%s", uri + strlen(prefix));
            //you can add a spesific name .....^ here if you want that
            gchar* outputFilePath = g_build_filename("downloads", filename, NULL);
            g_free(filename);

            // Download the URI using curl because downloading the URL using gtk download tools is a pain in the ass
            CURL* curl = curl_easy_init();
            if (curl) {
                FILE* outputFile = fopen(outputFilePath, "wb");
                if (outputFile) {
                    curl_easy_setopt(curl, CURLOPT_URL, uri);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);

                    CURLcode res = curl_easy_perform(curl);
                    fclose(outputFile);
                    //installs the file using flatpak install
                    if (res == CURLE_OK) {
                        g_print("Downloaded: %s\n", uri);
                        gchar* command = g_strdup_printf("flatpak install %s -y", uri, outputFilePath);
                        runCommand(command);
                        g_free(command);
                    } else {
                        g_print("Download failed: %s\n", curl_easy_strerror(res));
                    }
                } else {
                    g_print("Failed to open the output file.\n");
                }

                curl_easy_cleanup(curl);
            } else {
                g_print("Failed to initialize curl.\n");
            }

            g_free(outputFilePath);
        }
    }

    
}

void license()
{
    //license, i went lazy with "check LICENSE file", but i really dont care
    g_print("uFlathub-Desktop version 0.01, Copyright (C) 2023 Eren Aydın uFlathub-Desktop comes with ABSOLUTELY NO WARRANTY;\nfor details read the LICENSE file included with your download.\nThis is free software, and you are welcome to redistribute it under certain conditions;\nCheck the LICENSE file for details\n");

}

int main(int argc, char* argv[])
{
    //initializes gtk
    gtk_init(&argc, &argv); 
    license();
    //basic stuff for gtk, not gonna explain them
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Unofficial Flathub Desktop Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget* webView = webkit_web_view_new();
    //opens the main page for the webview
    const char* htmlFilePath = "website.html";
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), "https://flathub.org");

    // Connect the callback to the "decide-policy" signal
    g_signal_connect(webView, "decide-policy", G_CALLBACK(decide_policy_cb), NULL);

    gtk_container_add(GTK_CONTAINER(window), webView);

    gtk_widget_show_all(window);
    gtk_main();
      

    return 0;
}
