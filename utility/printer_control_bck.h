/* /usr2/utility/printer_control.h */
/* ------------------------------- */

#define MAX_DUPLICATE_TABLE     100

struct
{
        char *application;
        char *printer;
        char *printer_str;
        char *reset_str;
} printer_table[]=
        {
                /* Key */
                /* --- */
                "233report",
                "sam",
                        /* Control codes */
                        /* ------------- */
                        "~^15",
                        "^18",
                /* Key */
                /* --- */
                "233report",
                "oki1",
                        /* Control codes */
                        /* ------------- */
                        "~^15",
                        "^18",
                /* Key */
                /* --- */
                "210report",
                "comm",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "210report",
                "hp3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "210report",
                "pr3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "commrun",
                "comm",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "commrun",
                "hp3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "commrun",
                "pr3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "134report",
                "comm",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l0O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E",
                /* Key */
                /* --- */
                "134report",
                "hp3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&k2S",
                        "~E",
                /* Key */
                /* --- */
                "134report",
                "pr3",
                        /* Control codes */
                        /* ------------- */
                        "~E~&k2S",
                        "~E",
                /* Key */
                /* --- */
                "134report",
                "sam",
                        /* Control codes */
                        /* ------------- */
                        "^18",
                        "^18",
                /* Key */
                /* --- */
                "134report",
                "oki1",
                        /* Control codes */
                        /* ------------- */
                        "^18",
                        "^18",
                /* Key */
                /* --- */
                "80report",
                "comm",
                        /* Control codes */
                        /* ------------- */
                        "~E",
                        "~E",
                /* Key */
                /* --- */
                "80report",
                "hp3",
                        /* Control codes */
                        /* ------------- */
                        "~E",
                        "~E",
                /* Key */
                /* --- */
                "80report",
                "pr3",
                        /* Control codes */
                        /* ------------- */
                        "~E",
                        "~E",
                /* Delimiter */
                /* --------- */
                (char *)0,
                "",
                        "",
                        ""
        };


/* Operations */
/* ---------- */
char *get_printer_init_string( int index );
char *get_printer_reset_string( int index );

