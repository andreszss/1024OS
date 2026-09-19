void execute_command() {
    command_buffer[buffer_idx] = '\0';
    print("\n", 0x07);

    // COMANDOS:

    // HELP: Muestra la ayuda
    if (strcmp(command_buffer, "help") == 0) {
    print("Comandos:\n", 0x0A);
	print("help: muestra esto.\n", 0x0F);
	print("clear: limpiar la pantalla.\n", 0x0F);
	print("beep: suena un pitido.\n", 0x0F);
	print("fetch: muestra la info del sistema.\n", 0x0F);
	print("poweroff: apaga el equipo. (solo hipervisores: QEMU, VirtualBox/VMware, Bochs)\n", 0x0F);
	print("reboot: reinicia el equipo.\n", 0x0F);
	print("credits: muestra los creditos.\n", 0x0F);
	print("whoami: muestra quien eres.\n", 0x0F);
	print("echo: muestra lo que escribes.\n", 0x0F);
	print("changelog: muestra los cambios de las versiones.\n", 0x0F);
    } 

    // CLEAR: Limpiar pantalla
    else if (strcmp(command_buffer, "clear") == 0) {
        clear_screen();
        print("> ", 0x07);
    }

    // BEEP: Suena un pitido
    else if (strcmp(command_buffer, "beep") == 0) {
        print("BEEP!\n", 0x0A);
        beep(100);
    }

    // REBOOT: Reiniciar equipo
    else if (strcmp(command_buffer, "reboot") == 0) {
        outb(0x64, 0xFE);
    }

    // POWEROFF: Apagar equipo (hipervisores compatibles)
    else if (strcmp(command_buffer, "poweroff") == 0) {
        print("Apagando 1024OS...\n", 0x0C);
        outw(0x604, 0x2000);  // 1. QEMU (ACPI)
        outw(0x4004, 0x3400); // 2. VirtualBox/VMware
	outw(0xB004, 0x2000); // 3. Bochs/QEMU viejo
        outb(0x501, 0x31);    // 4. Cloud Hypervisor / QEMU (MicroVM)
        outw(0x8900, 0x8900); // 5. Bochs Debug Port
        print("Ahora es seguro apagar con el boton\n", 0x07);
        print("Sistema en estado halt\n", 0x0C);
        halt();
    }

    // FETCH: Muestra la información del sistema
    else if (strcmp(command_buffer, "fetch") == 0) {
		print(" _    ___   ____   _  _   \n", 0x0B);
        print("/ |  / _ \\ |___ \\ | || |  \n", 0x0B);
        print("| | | | | |  __) || || |_ \n", 0x0B);
        print("| | | |_| | / __/ |__   _|\n", 0x0B);
        print("|_|  \\___/ |_____|   |_|  \n", 0x0B);
        print("--------------------------------------\n", 0x07);
        print("OS:            1024OS v0.4-rc2\n", 0x0F);
        print("Kernel:        Pavilionix86 0.2\n", 0x0F);
        print("Shell:         mini-sh 0.1\n", 0x0F);
	print("Init System:   dvInit 0.2\n", 0x0F);
	print("Arch:          i386 (x86)\n", 0x0F);
    }

    // CREDITS: Muestra los creditos del sistema
    else if (strcmp(command_buffer, "credits") == 0) {
	   print("Hecho por: Andresqwq\n", 0x0B);
	   print("Gracias por probar 1024OS!\n", 0x0F);
	   beep(50);
    }

    // WHOAMI: Muestra quien eres
    else if (strcmp(command_buffer, "whoami") == 0) {
	print("root\n", 0x0F);
    }

    // ECHO : Imprime lo que escribes después de "echo "
    else if (strncmp(command_buffer, "echo ", 5) == 0) {
        // Imprimimos desde la posición 5 del búfer (después de "echo ")
        print(&command_buffer[5], 0x07);
        print("\n", 0x07);
    }
    else if (strcmp(command_buffer, "echo") == 0) {
        print("Uso: echo <mensaje>\n", 0x0E);
    }

    // CHANGELOG: Muestra los cambios de las versiones
    else if (strcmp(command_buffer, "changelog") == 0) {
    print("0.1:\n", 0x0A);
    print("- Primera version!!!\n", 0x0F);

    print("0.2:\n", 0x0A);
    print("- Reemplazado: Se elimino el bootloader GRUB por Syslinux.\n", 0x0F);
    print("- Corregido: Se corrigieron algunos bugs menores.\n", 0x0F);
    print("- Corregido: Se corrigio un bug de que el texto se quedaba sin pantalla.\n", 0x0F);
    print("- Corregido: Se reescribio el codigo y ahora es mas estable y ordenado.\n", 0x0F);
    print("- Añadido: Mas dispositivos compatible con poweroff.\n", 0x0F);
    print("- Añadido: Soporte para mayusculas.\n", 0x0F);
   
    print("0.3:\n", 0x0A);
    print("- Reemplazado: Se reemplazo el script de build, por uno mas robusto, mas\ndetallado, y mejor estructurado.\n", 0x0F);
    print("- Corregido: Arte ascii de 'fetch' ahora esta bien hecho.\n", 0x0F);
    print("- Corregido: Error al identificar la version de el sistema.\n", 0x0F);
    print("- Eliminado: Eliminado el comando 'matrix', se planea añadir uno mejor\nimplementado pronto.\n", 0x0F);

    print("0.4-rc1:\n", 0x0A);
    print("- Corregido: Reestructuracion de codigo fuente.\n", 0x0E);
    
    print("0.4-rc2:\n", 0x0A);
    print("- Corregido: Reestructuracion de codigo fuente.\n", 0x0E);
    print("- Corregido: README.md del proyecto cambiado.\n", 0x0E);
    print("- Corregido: Script de compilacion adaptado a la reestructuracion\nde codigo fuente.\n", 0x0E);
    print("Nota: Cada herramienta, se actualiza independientemente de las demas.\n", 0x0E);
    }

    // FIN DE COMANDOS


    // Si el comando no es reconocido y el buffer no está vacío, mostramos un error
    else if (buffer_idx > 0) {
        print("Comando desconocido.\n", 0x0C);
    }
    // Solo imprimimos el prompt si no limpiamos la pantalla
    if (strcmp(command_buffer, "clear") != 0) {
        print("-> ", 0x07);
    }
    buffer_idx = 0;
}
