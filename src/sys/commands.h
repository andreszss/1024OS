#ifndef COMMANDS_H
#define COMMANDS_H

#include <sys/hardware.h>

int last_exit_status = 0;

int parse_and_execute_single(char *cmd_buf) {
    trim(cmd_buf);
    if (cmd_buf[0] == '\0') return 0;

    char *eq = 0;
    for (int i = 0; cmd_buf[i] != '\0'; i++) {
        if (cmd_buf[i] == '=') {
            eq = &cmd_buf[i];
            break;
        }
    }

    if (eq != 0) {
        *eq = '\0';
        char *var_name = cmd_buf;
        char *var_val = eq + 1;
        trim(var_name);
        trim(var_val);
        set_env_var(var_name, var_val);
        return 0;
    }

    // HELP VARIABLES
    if (strcmp(cmd_buf, "help variables") == 0) {
        print_variables_help();
        return 0;
    }

    // HELP
    if (strcmp(cmd_buf, "help") == 0) {
        print("Comandos:\n", 0x0A);
        print("help: muestra esto.\n", 0x0F);
        print("help variables: muestra las variables de entorno.\n", 0x0F);
        print("clear: limpiar la pantalla.\n", 0x0F);
        print("beep: suena un pitido.\n", 0x0F);
        print("fetch: muestra la info del sistema.\n", 0x0F);
        print("poweroff: apaga el equipo.\n", 0x0F);
        print("reboot: reinicia el equipo.\n", 0x0F);
        print("credits: muestra los creditos.\n", 0x0F);
        print("whoami: muestra quien eres.\n", 0x0F);
        print("echo: muestra lo que escribes.\n", 0x0F);
        return 0;
    } 
    else if (strcmp(cmd_buf, "clear") == 0) {
        clear_screen();
        return 0;
    }
    else if (strcmp(cmd_buf, "beep") == 0) {
        print("BEEP!\n", 0x0A);
        beep(100);
        return 0;
    }
    else if (strcmp(cmd_buf, "reboot") == 0) {
        outb(0x64, 0xFE);
        return 0;
    }
    else if (strcmp(cmd_buf, "poweroff") == 0) {
        print("Apagando 1024OS...\n", 0x0C);
        outw(0x604, 0x2000);
        outw(0x4004, 0x3400);
        outw(0xB004, 0x2000);
        outb(0x501, 0x31);
        outw(0x8900, 0x8900);
        print("Sistema en estado halt\n", 0x0C);
        halt();
        return 0;
    }
    else if (strcmp(cmd_buf, "fetch") == 0) {
        char cpu_name[50];
        get_cpu_name(cpu_name);

        unsigned int total_ram = get_total_ram_mb();
        unsigned int used_ram = 2; 

        char uptime[20];
        get_uptime_string(uptime);

        char ram_used_str[10], ram_total_str[10];
        itoa(used_ram, ram_used_str);
        itoa(total_ram, ram_total_str);

        print(" _    ___   ____   _  _   \n", 0x0B);
        print("/ |  / _ \\ |___ \\ | || |  \n", 0x0B);
        print("| | | | | |  __) || || |_ \n", 0x0B);
        print("| | | |_| | / __/ |__   _|\n", 0x0B);
        print("|_|  \\___/ |_____|   |_|  \n", 0x0B);
        print("--------------------------------------\n", 0x07);
        print("OS:            1024OS v", 0x0F); print(get_env_var("version"), 0x0F); print("\n", 0x0F);
        print("Kernel:        Pavilionix86 0.2\n", 0x0F);
        print("Uptime:        ", 0x0F); print(uptime, 0x0F); print("\n", 0x0F);
        print("CPU:           ", 0x0F); print(cpu_name, 0x0F); print("\n", 0x0F);
        print("iGPU/GPU:      Generic VGA Compatible Adapter\n", 0x0F);
        print("RAM:           ", 0x0F); 
        print(ram_used_str, 0x0F); print(" MiB / ", 0x0F); 
        print(ram_total_str, 0x0F); print(" MiB\n", 0x0F);
        print("Pantalla:      VGA Text Mode\n", 0x0F);
        print("Arch:          i386 (x86)\n", 0x0F);
        return 0;
    }
    else if (strcmp(cmd_buf, "credits") == 0) {
        print("Hecho principalmente por: Andresqwq\n", 0x0B);
	print("Con ayuda de: ElmichiYT\n", 0x0B);
        beep(50);
        return 0;
    }
    else if (strcmp(cmd_buf, "whoami") == 0) {
        print("root\n", 0x0F);
        return 0;
    }
    else if (strncmp(cmd_buf, "echo ", 5) == 0) {
        print(&cmd_buf[5], 0x07);
        print("\n", 0x07);
        return 0;
    }
    else if (strcmp(cmd_buf, "echo") == 0) {
        print("Uso: echo <mensaje>\n", 0x0E);
        return 1;
    }

    print("Comando desconocido: ", 0x0C);
    print(cmd_buf, 0x0C);
    print("\n", 0x0C);
    return 1;
}

void execute_command() {
    command_buffer[buffer_idx] = '\0';
    print("\n", 0x07);

    if (buffer_idx > 0) {
        char expanded_buffer[256];
        expand_variables(command_buffer, expanded_buffer);

        char current_cmd[128];
        int c_idx = 0;
        int i = 0;

        while (expanded_buffer[i] != '\0') {
            if (expanded_buffer[i] == ';') {
                current_cmd[c_idx] = '\0';
                last_exit_status = parse_and_execute_single(current_cmd);
                c_idx = 0;
                i++;
            }
            else if (expanded_buffer[i] == '&' && expanded_buffer[i+1] == '&') {
                current_cmd[c_idx] = '\0';
                last_exit_status = parse_and_execute_single(current_cmd);
                c_idx = 0;
                i += 2;
                if (last_exit_status != 0) break;
            }
            else if (expanded_buffer[i] == '|' && expanded_buffer[i+1] == '|') {
                current_cmd[c_idx] = '\0';
                last_exit_status = parse_and_execute_single(current_cmd);
                c_idx = 0;
                i += 2;
                if (last_exit_status == 0) break;
            }
            else {
                current_cmd[c_idx++] = expanded_buffer[i++];
            }
        }

        if (c_idx > 0) {
            current_cmd[c_idx] = '\0';
            last_exit_status = parse_and_execute_single(current_cmd);
        }
    }

    if (strcmp(command_buffer, "clear") != 0) {
        print("-> ", 0x07);
    }
    buffer_idx = 0;
}

#endif
