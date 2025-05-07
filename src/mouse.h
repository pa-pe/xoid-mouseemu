#ifndef MOUSE_H
#define MOUSE_H

int move_mouse_relative(int dx, int dy);
int move_mouse_absolute(int x, int y);
int click_mouse(void);
//int scroll_mouse(int dx, int dy);
int scroll_mouse(int delta);

#endif // MOUSE_H
