#ifndef BALLTRAP_H
#define BALLTRAP_H

void balltrap_build(Dock *dock);
void balltrap_add(Dock *dock, id_type id);
void balltrap_animate(Dock *dock);
int balltrap_dispatch_event(Dock *dock, XEvent *ev);
void balltrap_check_message(id_type id, const unsigned char *msg);
void balltrap_launch();
void balltrap_airstrike(Dock *dock);
void balltrap_armageddon(Dock *dock);
#endif
