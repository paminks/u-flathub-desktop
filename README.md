# u-flathub-desktop
Unofficial Desktop Client for flathub.org

compile it with

```
g++ -o uFlathub main.cpp -std=c++11 -lcurl `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs webkit2gtk-4.0`
```

and run it with 
```
./uFlathub
```
