#ifndef VARIABLES_H
#define VARIABLES_H

typedef struct {
    char name[32];
    char value[64];
} EnvironmentVariable;

#define MAX_VARS 16
EnvironmentVariable env_vars[MAX_VARS];
int env_var_count = 0;

void get_cpu_vendor(char *buffer) {
    unsigned int eax, ebx, ecx, edx;
    asm volatile ("cpuid"
                  : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                  : "a"(0));

    ((unsigned int*)buffer)[0] = ebx;
    ((unsigned int*)buffer)[1] = edx;
    ((unsigned int*)buffer)[2] = ecx;
    buffer[12] = '\0';
}

void init_env_variables() {
    env_var_count = 0;

    strcpy(env_vars[0].name, "version");
    strcpy(env_vars[0].value, "0.4-rc3");

    strcpy(env_vars[1].name, "device");
    strcpy(env_vars[1].value, "PC x86 (Compatible)");

    strcpy(env_vars[2].name, "cpu_arch");
    get_cpu_vendor(env_vars[2].value);

    env_var_count = 3;
}

const char* get_env_var(const char *name) {
    for (int i = 0; i < env_var_count; i++) {
        if (strcmp(env_vars[i].name, name) == 0) {
            return env_vars[i].value;
        }
    }
    return "";
}

void set_env_var(const char *name, const char *value) {
    for (int i = 0; i < env_var_count; i++) {
        if (strcmp(env_vars[i].name, name) == 0) {
            strcpy(env_vars[i].value, value);
            return;
        }
    }
    if (env_var_count < MAX_VARS) {
        strcpy(env_vars[env_var_count].name, name);
        strcpy(env_vars[env_var_count].value, value);
        env_var_count++;
    }
}

void expand_variables(const char *input, char *output) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        if (input[i] == '$') {
            i++;
            char var_name[32];
            int vk = 0;
            while (input[i] != '\0' && input[i] != ' ' && input[i] != ';' && input[i] != '&' && input[i] != '|' && input[i] != '$') {
                var_name[vk++] = input[i++];
            }
            var_name[vk] = '\0';

            const char *val = get_env_var(var_name);
            while (*val) {
                output[j++] = *val++;
            }
        } else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';
}

void print_variables_help() {
    print("Variables de entorno disponibles:\n", 0x0A);
    for (int i = 0; i < env_var_count; i++) {
        print("  $", 0x0B);
        print(env_vars[i].name, 0x0B);
        print(" = ", 0x07);
        print(env_vars[i].value, 0x0F);
        print("\n", 0x07);
    }
    print("\nPuedes crear nuevas variables escribiendo: VAR=valor\n", 0x0E);
}

#endif
