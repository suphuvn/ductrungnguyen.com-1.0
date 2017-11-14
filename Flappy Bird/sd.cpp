#include <stdio.h>

struct song { char title[40]; int rating; };

void initSongs( song &mySong);

int main() {
	song mySong[5];
	initSongs(mySong);
}

void initSongs(song &s) {
     for(int i=0; i<5; i++) {
	strcpy(s[i].title, "");
	s[i].rating = 0;
    }
}
