#include QMK_KEYBOARD_H
#include "mousekey.h"

enum Layers {
    DEFAULT,
    MARK,
    FUNC
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
      +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
      |  Tab  |   Q   |   W   |   E   |   R   |   T   |   Y   |   U   |   I   |   O   |   P   |  Bs   |
      +-----------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------+
      |   Ctrl    |   A   |   S   |   D   |   F   |   G   |   H   |   J   |   K   |   L   |   Shift   |
      +---------+---------+-------+-------+-------+-------+-------+-------+-------+---------+---------+
      |  Shift  |  Alt    |   Z   |   X   |   C   |   V   |   B   |   N   |   M   |  Super  |  Enter  |
      +---------+---------+---+-------+-------+---------------+-------+-------+---+---------+---------+
                              |  Esc  | Super |    LClick &   |  MARK |  FUNC |
                              +-------+-------\ Mouse Cursor  /-------+-------+
                                               +-------------+
     */
    [DEFAULT] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_RSFT,
        KC_LSFT, KC_LALT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_RGUI, KC_ENT,
                                   KC_ESC,  KC_LGUI, KC_BTN1,          MO(MARK),MO(FUNC)
    ),
    /*
      +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
      |       |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   0   |       |
      +-----------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------+
      |           |   |   |   _   |   +   |   {   |   }   | Left  | Down  |  Up   | Right |           |
      +---------+---------+-------+-------+-------+-------+-------+-------+-------+-------+-----------+
      |         |         |   ~   |   :   |   "   | Space |   <   |   >   |   ?   |         |         |
      +---------+---------+---+-------+-------+---------------+-------+-------+---+---------+---------+
                              |       |       |    RClick &   |XXXXXXX|XXXXXXX|
                              +-------+-------\  Mouse Scroll /-------+-------+
                                               +-------------+
     */
    [MARK] = LAYOUT(
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
        _______, KC_BSLS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______,
        _______, _______, KC_GRV,  KC_SCLN, KC_QUOT, KC_SPC,  KC_COMM, KC_DOT,  KC_SLSH, _______, _______,
                                   _______, _______, KC_BTN2,          XXXXXXX, XXXXXXX
    ),
    /*
      +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
      |       |  F1   |  F2   |   F3  |   F4  |   F5  | Mute  |VolDown| VolUp |XXXXXXX|XXXXXXX|       |
      +-----------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------+
      |           |  F6   |  F7   |  F8   |  F9   |  Esc  | Left  | Down  |  UP   | Right |           |
      +---------+---------+-------+-------+-------+-------+-------+-------+-------+-------+-----------+
      |         |         |  F10  |  F11  |  F12  | Home  |  End  | PgDn  | PgUp  |         |         |
      +---------+---------+---+-------+-------+---------------+-------+-------+---+---------+---------+
                              |       |       |    RClick &   |XXXXXXX|XXXXXXX|
                              +-------+-------\  Mouse Scroll /-------+-------+
                                               +-------------+
     */
    [FUNC] = LAYOUT(
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_MUTE, KC_VOLD, KC_VOLU, XXXXXXX, XXXXXXX, _______,
        _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_ESC,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______,
        _______, _______, KC_F10,  KC_F11,  KC_F12,  KC_HOME, KC_END,  KC_PGDN, KC_PGUP, _______, _______,
                                   _______, _______, KC_BTN2,          XXXXXXX, XXXXXXX
    )
};

// レイヤーキーが押されている間は、ポインティングデバイスの操作で画面スクロールするようにする
bool enable_scroll = false;

layer_state_t layer_state_set_user(layer_state_t state)
{
    if(state != 0){
        enable_scroll = true;
    } else {
        enable_scroll = false;
    }

    return state;
}

const uint32_t cursor_interval = 10;
const uint32_t scroll_interval = 80;

const uint8_t cursor_max_speed = 5;
const uint8_t wheel_max_speed  = 2;

const uint8_t cursor_speed_regulator = 10;
const uint8_t wheel_speed_regulator  = 15;

// from drivers/sensors/analog_joystick.c
extern int16_t xOrigin, yOrigin;
extern int16_t axisCoordinate(pin_t, uint16_t);

int8_t my_axisToMouseComponent(pin_t pin, int16_t origin, uint8_t maxSpeed, uint8_t regulator)
{
    int16_t coordinate = axisCoordinate(pin, origin);
    if (coordinate != 0) {
        float percent = (float)coordinate / 100;
        return percent * maxSpeed * (abs(coordinate) / regulator);
    } else {
        return 0;
    }
}

void pointing_device_task(void)
{
    // 最後にマウスカーソルの情報を送った時間
    static uint32_t last_cursor = 0;
    // 最後にマウスホイールの情報を送った時間
    static uint32_t last_scroll = 0;

    report_mouse_t report = {};

    if(enable_scroll) {
        if(timer_elapsed32(last_scroll) < scroll_interval) {
            return;
        }
        last_scroll = timer_read32();

        report.h = my_axisToMouseComponent(ANALOG_JOYSTICK_X_AXIS_PIN, xOrigin, wheel_max_speed, wheel_speed_regulator);
        report.v = my_axisToMouseComponent(ANALOG_JOYSTICK_Y_AXIS_PIN, yOrigin, wheel_max_speed, wheel_speed_regulator);
    } else {
        if(timer_elapsed32(last_cursor) < cursor_interval) {
            return;
        }
        last_cursor = timer_read32();

        report.x =   my_axisToMouseComponent(ANALOG_JOYSTICK_X_AXIS_PIN, xOrigin, cursor_max_speed, cursor_speed_regulator);
        // スティックを上下に倒すと、カーソルが逆方向に動くので修正(スクロールは問題なし)
        report.y = - my_axisToMouseComponent(ANALOG_JOYSTICK_Y_AXIS_PIN, yOrigin, cursor_max_speed, cursor_speed_regulator);
    }

    // こうしないとマウスキーのボタンが効かなくなるっぽい
    report.buttons = mousekey_get_report().buttons;

    pointing_device_set_report(report);
    pointing_device_send();
}
