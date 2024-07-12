typedef enum {JOYSTICK_UP, JOYSTICK_DOWN, JOYSTICK_LEFT, JOYSTICK_RIGHT, JOYSTICK_CENTER, UNDEFINED} Joystick_Status_Enum;
extern void Joystick_Initialize(void);
extern Joystick_Status_Enum Joystick_Get_Status(void);
