int cli_execute(int argc, const char *const *argv);
void cli_print_help(const char *const *argv);
void cli_print_name(const char *const *argv);
void cli_print_ver(const char *const *argv);
void cli_print_ascii_tbl(const char *const *argv);
void cli_month(const char *const *argv);
void cli_print_cmd_error(void);
void cli_print_cmd_arg_error(void);
void cli_example(const char *const *argv);
void cli_mem(const char *const *argv);
void cli_rfid_read(const char *const *argv);
void cli_rfid_add(const char *const *argv);
void cli_rfid_print(const char *const *argv);
void cli_remove_card(const char *const *argv);
void print_bytes(const uint8_t *array, const size_t len);

typedef enum {
    door_opening,
    door_open,
    door_closing,
    door_closed
} door_state_t;

typedef enum {
    display_name,
    display_access_denied,
    display_clear,
    display_no_updates,
} display_state_t;

display_state_t previous_display_state;

typedef struct cli_cmd {
    PGM_P cmd;
    PGM_P help;
    void (*func_p)();
    const uint8_t func_argc;
} cli_cmd_t;