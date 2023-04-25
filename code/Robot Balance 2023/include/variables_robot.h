
float target_speed_degrees_per_second_moteur_1 = 90;
int ratio_moteur_1 = 4;
int step_per_tour_moteur_1 = 200;
int delai_timer_moteur_1 = 1000000;

float target_speed_degrees_per_second_moteur_2 = 45;
int ratio_moteur_2 = 4;
int step_per_tour_moteur_2 = 200;
int delai_timer_moteur_2 = 1000000;


int angle_index = 0;
int angle_samples[10];
float angle_average = 0;

unsigned long previousMillisControlLoop1;
unsigned long previousMillisControlLoop2;

float angle_set_point = 5;
int angle = 0;
float vitesse = 0;

float absolute_position_set_point = 0;
int32_t absolute_position = 0;
float absolute_position_error = 0;

float angle_erreur = 0;
float angle_erreur_somme = 0;

#define KP_SPEED 0.001
#define DIAMETRE_ROUE 0.91

#define KP 75
#define KI 100
float dt = 0.02;

int angle_sp = 8;
int erreur = 0;
int output = 0;
