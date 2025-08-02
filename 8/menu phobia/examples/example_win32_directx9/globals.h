#pragma once

/*icons:
A - target (person)
B - target variant 2
C - eye
D - half filled( or half empty :) ) circle
E - circle
F - flag
G - heart
H - home
I - linux
J - magic stick
K - 'play'
L - exit
M - exit variant 2
N - wifi
O - half filled shield
P - log out
Q - loading
R - water drop
S - umbrella
T - caution
U - settings
*/

#define cheat_icon_symbol "G"
#define cheat_name "half-life"
#define cheat_domain ".dev"

bool particles = true;

/*floats*/
float accent_colour[4] = { 255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f };

/*ints*/
int old_tab = 0;
static int tab = 0;

/*chars*/
static char username[64];
static char password[64];

/*options*/
float content_animation = 0.0f;
float dpi_scale = 1.0f;

static bool update_on_f5 = true;
static bool remember_me = false;

static int game = 0;
const char* games_list[4] = { "Counter-Strike: GO", "Dying Light", "Apex Legends", "Warface" };
