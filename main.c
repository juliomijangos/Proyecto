/*
============================================================
  DOCUMENTACION DEL CODIGO:
  Nombre:       Proyecto-Sistemas SAGA
  Compilador:   Dev C++
  Programadores:
                Julio Cesar  Mijangos Irineo
                Anna Fernanda Orozco Pozos
                Daniel Salcedo Miranda
                ELian Guillermo Pereira Canul
                Perla Noemi Canche Uicab
  Fecha:        16 de mayo del 2026
  Version:      1.0

  DESCRIPCION GENERAL DEL SISTEMA:
  SAGA es un sistema de gestion de activos fijos. Permite
  registrar maquinaria, equipo u otros bienes de una empresa,
  calcular su depreciacion con el tiempo, actualizar su estado,
  registrar mantenimientos y generar reportes. Todo se guarda
  en archivos binarios (.dat) para que los datos persistan
  aunque el programa se cierre.
============================================================
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>


/* ==============================================================
   SECCION 1: ESTRUCTURAS DE DATOS
   Una estructura (struct) agrupa varios datos relacionados
   en un solo "molde". Aqui definimos 3 estructuras:
   - ActivoFijo: representa un bien de la empresa
   - RegistroMantenimiento: representa un evento de mantenimiento
   - BitacoraEvento: representa una accion registrada en el sistema
   ============================================================== */

/* ---- ActivoFijo ----
   Cada variable de este tipo representa UN activo .
   typedef permite usar "ActivoFijo" como nombre de tipo
   directamente, sin escribir "struct" cada vez. */
typedef struct {
    char   id[15];           /* Identificador unico */
    char   nombre[50];       /* Nombre descriptivo */
    char   descripcion[100]; /* Descripcion adicional del activo */
    double costo_inicial;    /* Precio de compra original */
    double valor_residual;   /* Valor minimo al que puede llegar al depreciarse */
    int    vida_util;        /* Cuantos años se espera que dure el activo */
    int    estado;           /* Estado actual:
                                1 = Operativo
                                2 = En mantenimiento
                                3 = Depreciado
                                4 = Dado de baja (eliminacion logica) */
    time_t fecha_ingreso;    /* Fecha en que fue registrado en el sistema.*/
                              
    } ActivoFijo;

/* ---- RegistroMantenimiento ----
   Cada vez que un activo recibe mantenimiento, se crea
   un registro de este tipo y se guarda en mantenimiento.dat */
typedef struct {
    char   id_activo[15];        /* ID del activo al que corresponde */
    char   descripcion[150];     /* Que se hizo en el mantenimiento */
    double costo_mantenimiento;  /* Cuanto costo el mantenimiento */
    time_t fecha_mantenimiento;  /* Fecha en que ocurrio */
    int    tipo;                 /* 1 = Preventivo (programado)
                                    2 = Correctivo (por falla) */
} RegistroMantenimiento;

/* ---- BitacoraEvento ----
   Cada accion importante (alta, baja, cambio de estado, etc.)
   queda registrada aqui. Sirve como historial de auditoria. */
typedef struct {
    char   id_activo[15]; /* ID del activo involucrado */
    char   usuario[30];   /* Quien realizo la accion (Admin / Operador) */
    char   accion[100];   /* Descripcion de lo que se hizo */
    time_t fecha_hora;    /* Fecha y hora exacta del evento */
} BitacoraEvento;


/* ==============================================================
   SECCION 2: PROTOTIPOS DE FUNCIONES
   ============================================================== */

/* Menus de navegacion */
void menu_principal();
void menu_administrador();
void menu_Operador_Almacen();

/*Menus para ambos perfiles*/
void menu_mantenimiento();
void menu_estadisticas();

/* Funciones del administrador */
void altaActivo();
void generarReporte();
void eliminarActivo();
void reporte_de_texto();
void buscar_por_nombre();
void exportar_reporte_csv();

/* Funciones del operador de almacen */
void consultar_activo();
void mostrar_inventario();
void Actualizar_activo();
void Monitoreo_de_ciclo_de_vida_del_activo();
void Historial_Movimiento();
void filtrar_por_estado();

/* Funciones de mantenimiento */
void registrar_mantenimiento();
void ver_historial_mantenimiento();
void activos_en_mantenimiento();

/* Funciones de estadisticas para ambo perfiles */
void estadisticas_generales();
void resumen_depreciacion();
void activos_proximos_depreciar();

/* Funciones de bitacora para ambos perfiles */
void registrar_evento(const char *id_activo, const char *usuario, const char *accion);
void ver_bitacora();

/* Funciones utilitarias (herramientas de apoyo) */
void limpiar_buffer();
void pausar();
const char* estado_texto(int estado);
double calcular_valor_actual(ActivoFijo *a);
double calcular_depreciacion_anual(ActivoFijo *a);
void imprimir_separador(char c, int n);


/* ==============================================================
   SECCION 3: FUNCION MAIN
   ============================================================== */
int main(){
    menu_principal();
    return 0; 
}


/* ==============================================================
   SECCION 4: MENUS DE NAVEGACION
   Cada menu muestra opciones al usuario y redirige a la
   funcion correspondiente segun lo que elija.
   Todos usan un ciclo do-while para seguir mostrando el menu
   hasta que el usuario elija "Salir" o "Volver".
   ============================================================== */

/* ---- menu_principal ----
   Es el primer menu que ve el usuario al iniciar el programa.
   Divide el sistema en 4 roles/modulos principales. */
void menu_principal(){
    int opcion;

    do{
        printf("\n");
        imprimir_separador('=', 42);
        printf("|| Sistema de gestion de activos SAGA  ||\n");
        imprimir_separador('=', 42);
        printf("||  1.- Modo administrador             ||\n");
        printf("||  2.- Operador de almacen            ||\n");
        printf("||  3.- Gestion de mantenimiento       ||\n");
        printf("||  4.- Estadisticas y analisis        ||\n");
        printf("||  5.- Ver bitacora del sistema       ||\n");
        printf("||  6.- Salir                          ||\n");
        imprimir_separador('=', 42);
        printf("Seleccione una opcion: ");

        /* Si el usuario escribe algo que no es un numero,
           scanf devuelve 0 (fallo). En ese caso limpiamos
           el buffer y repetimos el ciclo con "continue". */
        if(scanf("%d", &opcion) != 1){
            limpiar_buffer(); //llamada a una funcion de apoyo
            continue;
        }
        limpiar_buffer(); /* Limpiar el '\n' que queda en el buffer tras scanf */

        switch(opcion){
            case 1: menu_administrador();    break;
            case 2: menu_Operador_Almacen(); break;
            case 3: menu_mantenimiento();    break;
            case 4: menu_estadisticas();     break;
            case 5: ver_bitacora();          break;
            case 6: printf("\nCerrando sistema SAGA. Hasta luego.\n\n"); break;
            default: printf("\n[!] Opcion invalida.\n");
        }

    } while(opcion != 6); /* El programa termina cuando el usuario elige 6 */
}

/* ---- menu_administrador ----
   Opciones reservadas para el administrador del sistema.
   Puede registrar, eliminar y generar reportes de activos. */
void menu_administrador(){
    int opcion;

    do{
        printf("\n");
        imprimir_separador('+', 38);  //llamada la funcion de apoyo
        printf("|      Menu de Administrador         |\n");
        imprimir_separador('+', 38);
        printf("| 1.- Registrar nuevo activo         |\n");
        printf("| 2.- Generar reporte de inversion   |\n");
        printf("| 3.- Dar de baja un activo          |\n");
        printf("| 4.- Generar reporte de texto       |\n");
        printf("| 5.- Buscar activo por nombre       |\n");
        printf("| 6.- Exportar reporte a CSV         |\n");
        printf("| 7.- Volver                         |\n");
        imprimir_separador('+', 38);
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        limpiar_buffer();

        //en base la opcion del perfil se llamara una funcion
        switch(opcion){
            case 1: altaActivo();           break;
            case 2: generarReporte();       break;
            case 3: eliminarActivo();       break;
            case 4: reporte_de_texto();     break;
            case 5: buscar_por_nombre();    break;
            case 6: exportar_reporte_csv(); break;
            case 7: return; /* "return" sale de la funcion, volviendo al menu anterior */
            default: printf("\n[!] Opcion no valida.\n");
        }

    } while(opcion != 7);
}

/* ---- menu_Operador_Almacen ----
   Opciones para el personal que maneja el inventario fisico.
   Puede consultar, ver y actualizar activos, pero no darlos de baja. */
void menu_Operador_Almacen(){
    int ops;

    do{
        printf("\n");
        imprimir_separador('+', 42);
        printf("|         Menu de Operador de Almacen  |\n");
        imprimir_separador('+', 42);
        printf("|  1. Consultar activo por ID          |\n");
        printf("|  2. Mostrar todo el inventario       |\n");
        printf("|  3. Actualizar estado de activo      |\n");
        printf("|  4. Monitoreo de ciclo de vida       |\n");
        printf("|  5. Ver historial de movimientos     |\n");
        printf("|  6. Filtrar activos por estado       |\n");
        printf("|  7. Volver                           |\n");
        imprimir_separador('+', 42);
        printf("Seleccione una opcion: ");
        scanf("%d", &ops);
        limpiar_buffer();

        switch(ops){
            case 1: consultar_activo();                      break;
            case 2: mostrar_inventario();                    break;
            case 3: Actualizar_activo();                     break;
            case 4: Monitoreo_de_ciclo_de_vida_del_activo(); break;
            case 5: Historial_Movimiento();                  break;
            case 6: filtrar_por_estado();                    break;
            case 7: return;
            default: printf("\n[!] Opcion no valida.\n");
        }

    } while(ops != 7);
}

/* ---- menu_mantenimiento ----
   Permite registrar y consultar eventos de mantenimiento
   de los activos (preventivo o correctivo). */
void menu_mantenimiento(){
    int opcion;

    do{
        printf("\n");
        imprimir_separador('+', 40);
        printf("|      Menu de Mantenimiento           |\n");
        imprimir_separador('+', 40);
        printf("|  1. Registrar mantenimiento          |\n");
        printf("|  2. Ver historial de mantenimiento   |\n");
        printf("|  3. Activos en mantenimiento         |\n");
        printf("|  4. Volver                           |\n");
        imprimir_separador('+', 40);
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        limpiar_buffer();

        switch(opcion){
            case 1: registrar_mantenimiento();     break;
            case 2: ver_historial_mantenimiento(); break;
            case 3: activos_en_mantenimiento();    break;
            case 4: return;
            default: printf("\n[!] Opcion no valida.\n");
        }

    } while(opcion != 4);
}

/* ---- menu_estadisticas ----
   Muestra informacion resumida y de analisis sobre
   el estado financiero y fisico del inventario. */
void menu_estadisticas(){
    int opcion;

    do{
        printf("\n");
        imprimir_separador('+', 40);
        printf("|      Menu de Estadisticas            |\n");
        imprimir_separador('+', 40);
        printf("|  1. Estadisticas generales           |\n");
        printf("|  2. Resumen de depreciacion          |\n");
        printf("|  3. Activos proximos a depreciar     |\n");
        printf("|  4. Volver                           |\n");
        imprimir_separador('+', 40);
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        limpiar_buffer();

        switch(opcion){
            case 1: estadisticas_generales();     break;
            case 2: resumen_depreciacion();       break;
            case 3: activos_proximos_depreciar(); break;
            case 4: return;
            default: printf("\n[!] Opcion no valida.\n");
        }

    } while(opcion != 4);
}


/* ==============================================================
   SECCION 5: FUNCIONES DEL ADMINISTRADOR
   ============================================================== */

/* ---- altaActivo ----
   Registra un nuevo activo fijo en el sistema.
   - Pide los datos al usuario con scanf/fgets.
   - Asigna memoria dinamica con malloc para el nuevo activo.
   - Guarda el activo en "activos.dat" en modo "ab"
     (append binario: agrega al final sin borrar lo anterior).
   - Libera la memoria con free al terminar. */
void altaActivo(){

    /* malloc reserva memoria del heap para un ActivoFijo.
       Devuelve un puntero; si falla, devuelve NULL. */
    ActivoFijo *nuevo = (ActivoFijo *)malloc(sizeof(ActivoFijo));

    if(nuevo == NULL){
        printf("Error: No se pudo asignar memoria.\n");
        return;
    }

    printf("\n--- Registro de Activo Fijo ---\n");

    printf("ID unico (max 14 chars): ");
    fgets(nuevo->id, 15, stdin);
    /* fgets incluye el '\n' al final; strcspn lo localiza
       y lo reemplaza con '\0' (terminador de cadena) */
    nuevo->id[strcspn(nuevo->id, "\n")] = 0;

    printf("Nombre del activo: ");
    fgets(nuevo->nombre, 50, stdin);
    nuevo->nombre[strcspn(nuevo->nombre, "\n")] = 0;

    printf("Descripcion: ");
    fgets(nuevo->descripcion, 100, stdin);
    nuevo->descripcion[strcspn(nuevo->descripcion, "\n")] = 0;

    printf("Costo inicial: $");
    scanf("%lf", &nuevo->costo_inicial);  /* %lf lee un double */

    printf("Valor residual: $");
    scanf("%lf", &nuevo->valor_residual);

    printf("Vida util (anios): ");
    scanf("%d", &nuevo->vida_util);
    limpiar_buffer();

    nuevo->estado = 1;               /* Por defecto: Operativo */
    nuevo->fecha_ingreso = time(NULL); /* Obtiene la fecha/hora actual del sistema */

    /* Abrir el archivo en modo "ab": append binario.
       Si no existe, lo crea. Si ya existe, agrega al final. */
    FILE *archivo = fopen("activos.dat", "ab");
    if(archivo != NULL){
        /* fwrite escribe el bloque de memoria del struct al archivo */
        fwrite(nuevo, sizeof(ActivoFijo), 1, archivo);
        fclose(archivo);
        printf("\n[OK] Activo '%s' registrado exitosamente.\n", nuevo->nombre);
        /* Registrar en bitacora que se dio de alta un activo */
        registrar_evento(nuevo->id, "Admin", "Alta de activo");
    } else {
        printf("Error al abrir el archivo.\n");
    }

    free(nuevo); /* Liberar la memoria que reservamos con malloc */
}

/* ---- generarReporte ----
   Lee todos los activos del archivo y muestra una tabla
   con su ID, nombre, costo original y valor actual calculado.
   Omite los activos dados de baja (estado == 4). */
void generarReporte(){
    ActivoFijo temp; /* Variable temporal donde se carga cada activo leido */
    FILE *archivo = fopen("activos.dat", "rb"); /* "rb" = leer en binario */

    if(archivo == NULL){
        printf("No hay datos registrados.\n");
        return;
    }

    printf("\n");
    imprimir_separador('-', 70);
    /* %-15s = texto alineado a la izquierda con ancho de 15 caracteres */
    printf("%-15s %-20s %-12s %-12s %-10s\n",
           "ID", "Nombre", "Costo Inic.", "Valor Act.", "Estado");
    imprimir_separador('-', 70);

    /* fread lee un struct completo del archivo cada iteracion.
       Devuelve 1 si leyo correctamente, 0 si llego al final. */
    while(fread(&temp, sizeof(ActivoFijo), 1, archivo)){
        if(temp.estado == 4) continue; /* Saltar activos dados de baja */
        double valor_actual = calcular_valor_actual(&temp);
        printf("%-15s %-20s $%-11.2f $%-11.2f %-10s\n",
               temp.id, temp.nombre, temp.costo_inicial,
               valor_actual, estado_texto(temp.estado));
    }

    imprimir_separador('-', 70);
    fclose(archivo);
}

/* ---- eliminarActivo ----
   Hace una "eliminacion logica" del activo: en lugar de borrar
   el registro del archivo (lo cual es complicado en binario),
   simplemente cambia su estado a 4 (Baja). Asi sigue en el
   archivo pero el sistema lo trata como inexistente.
   Usa fseek para retroceder al inicio del struct ya leido
   y sobreescribirlo con el estado actualizado. */
void eliminarActivo(){

    char id_buscar[15];
    ActivoFijo temp;
    int encontrado = 0; //variable para busqueda

    /* "rb+" = leer Y escribir en binario (sin truncar el archivo) */
    FILE *archivo = fopen("activos.dat", "rb+");
    if(archivo == NULL){
        printf("\n[!] No existe el inventario registrado.\n");
        return;
    }

    printf("\n---------- Dar de baja activo ----------\n");
    printf("Ingrese la ID del activo: ");
    scanf("%s", id_buscar);
    limpiar_buffer();

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){

        if(strcmp(temp.id, id_buscar) == 0){ /* strcmp compara dos cadenas; 0 = iguales */
            encontrado = 1;

            if(temp.estado == 4){
                printf("\n[!] El activo '%s' ya fue dado de baja.\n", temp.nombre);
                break;
            }

            printf("\n[*] Activo encontrado: %s\n", temp.nombre);
            temp.estado = 4; /* Marcamos como dado de baja */

            /* fseek mueve el cursor del archivo.
               Como fread ya avanzo sizeof(ActivoFijo) bytes,
               retrocedemos esa misma cantidad para sobreescribir
               el mismo registro que acabamos de leer. */
            fseek(archivo, -(long)sizeof(ActivoFijo), SEEK_CUR);
            fwrite(&temp, sizeof(ActivoFijo), 1, archivo);

            printf("[OK] Activo dado de baja exitosamente.\n");
            registrar_evento(id_buscar, "Admin", "Baja de activo");
            break;
        }
    }

    if(encontrado == 0)
        printf("\n[!] Activo no encontrado.\n");

    fclose(archivo);
}

/* ---- reporte_de_texto ----
   Genera un archivo "reporte.txt" con todos los datos
   de cada activo formateados de forma legible. Tambien
   calcula el total de activos y la inversion total activa.
   fprintf funciona igual que printf pero escribe en un archivo. */
void reporte_de_texto(){
    ActivoFijo temp;
    FILE *archivo = fopen("activos.dat", "rb");
    FILE *reporte  = fopen("reporte.txt", "w"); /* "w" = crear/sobreescribir en texto */

    if(archivo == NULL){
        printf("\n[!] No hay inventario registrado.\n");
        if(reporte) fclose(reporte);
        return;
    }
    if(reporte == NULL){
        printf("\n[!] No se pudo crear el archivo de reporte.\n");
        fclose(archivo);
        return;
    }

    fprintf(reporte, "===== REPORTE DE ACTIVOS FIJOS - SISTEMA SAGA =====\n\n");
    int contador = 0;
    double total_inversion = 0.0;

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        double valor_actual = calcular_valor_actual(&temp);

        fprintf(reporte, "ID:             %s\n", temp.id);
        fprintf(reporte, "Nombre:         %s\n", temp.nombre);
        fprintf(reporte, "Descripcion:    %s\n", temp.descripcion);
        fprintf(reporte, "Costo inicial:  $%.2f\n", temp.costo_inicial);
        fprintf(reporte, "Valor residual: $%.2f\n", temp.valor_residual);
        fprintf(reporte, "Valor actual:   $%.2f\n", valor_actual);
        fprintf(reporte, "Depr. anual:    $%.2f\n", calcular_depreciacion_anual(&temp));
        fprintf(reporte, "Vida util:      %d anios\n", temp.vida_util);
        fprintf(reporte, "Estado:         %s\n", estado_texto(temp.estado));
        fprintf(reporte, "Fecha ingreso:  %s", ctime(&temp.fecha_ingreso));
        fprintf(reporte, "--------------------------------------------\n");

        contador++;
        if(temp.estado != 4) total_inversion += temp.costo_inicial;
    }

    fprintf(reporte, "\nTotal de activos registrados: %d\n", contador);
    fprintf(reporte, "Inversion total activa: $%.2f\n", total_inversion);

    fclose(archivo);
    fclose(reporte);
    printf("\n[OK] Reporte generado en 'reporte.txt' (%d activos).\n", contador);
}

/* ---- buscar_por_nombre ----
   Permite buscar activos escribiendo solo una parte del nombre.
   Usa strstr(), que devuelve un puntero si encuentra la subcadena,
   o NULL si no la encuentra. Asi no hace falta escribir el nombre
   completo y exacto. */
void buscar_por_nombre(){

    char nombre_buscar[50];
    ActivoFijo temp;
    int encontrados = 0;

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("\n[!] No hay inventario registrado.\n");
        return;
    }

    printf("\n===== BUSQUEDA POR NOMBRE =====\n");
    printf("Ingrese el nombre o parte del nombre: ");
    fgets(nombre_buscar, 50, stdin);
    nombre_buscar[strcspn(nombre_buscar, "\n")] = 0;

    printf("\nResultados:\n");
    imprimir_separador('-', 50);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        /* strstr(cadena_donde_buscar, subcadena_a_buscar)
           Ejemplo: strstr("Laptop Dell", "Dell") != NULL -> encontrado */
        if(strstr(temp.nombre, nombre_buscar) != NULL){
            printf("ID: %-15s Nombre: %-20s Estado: %s\n",
                   temp.id, temp.nombre, estado_texto(temp.estado));
            encontrados++;
        }
    }

    if(encontrados == 0)
        printf("[!] Ningun activo coincide con '%s'.\n", nombre_buscar);
    else
        printf("\n[OK] %d activo(s) encontrado(s).\n", encontrados);

    fclose(archivo);
}

/* ---- exportar_reporte_csv ----
   Crea un archivo "activos_reporte.csv" que puede abrirse
   en Excel u hojas de calculo. Un CSV es texto plano donde
   cada campo se separa por coma y cada activo ocupa una linea.
   La primera linea es el encabezado de columnas. */
void exportar_reporte_csv(){
    ActivoFijo temp;
    FILE *archivo = fopen("activos.dat", "rb");
    FILE *csv     = fopen("activos_reporte.csv", "w");

    if(archivo == NULL){
        printf("\n[!] No hay inventario registrado.\n");
        if(csv) fclose(csv);
        return;
    }
    if(csv == NULL){
        printf("\n[!] No se pudo crear el archivo CSV.\n");
        fclose(archivo);
        return;
    }

    /* Primera linea del CSV: nombres de columnas */
    fprintf(csv, "ID,Nombre,Descripcion,Costo Inicial,Valor Residual,"
                 "Valor Actual,Depreciacion Anual,Vida Util,Estado,Fecha Ingreso\n");

    int contador = 0;
    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        double va = calcular_valor_actual(&temp);
        double da = calcular_depreciacion_anual(&temp);

        /* ctime devuelve una cadena con la fecha, pero incluye '\n'
           al final; la quitamos para que no rompa el CSV */
        char fecha_str[30];
        strncpy(fecha_str, ctime(&temp.fecha_ingreso), 29);
        fecha_str[strcspn(fecha_str, "\n")] = 0;

        fprintf(csv, "%s,%s,%s,%.2f,%.2f,%.2f,%.2f,%d,%s,%s\n",
                temp.id, temp.nombre, temp.descripcion,
                temp.costo_inicial, temp.valor_residual,
                va, da, temp.vida_util,
                estado_texto(temp.estado), fecha_str);
        contador++;
    }

    fclose(archivo);
    fclose(csv);
    printf("\n[OK] CSV exportado en 'activos_reporte.csv' (%d registros).\n", contador);
}


/* ==============================================================
   SECCION 6: FUNCIONES DEL OPERADOR DE ALMACEN
   ============================================================== */

/* ---- consultar_activo ----
   Busca un activo por su ID exacto y muestra todos sus datos,
   incluyendo el valor actual calculado en tiempo real. */
void consultar_activo(){
    char id_buscar[15];
    int encontrado = 0;
    ActivoFijo temp;

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("\nNo existe inventario registrado.\n");
        return;
    }

    printf("\n========== CONSULTAR ACTIVO ==========\n");
    printf("Ingrese el ID del activo: ");
    scanf("%s", id_buscar);
    limpiar_buffer();

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(strcmp(temp.id, id_buscar) == 0){
            encontrado = 1;

            /* Calculamos datos en tiempo real para mostrar */
            double va = calcular_valor_actual(&temp);
            double da = calcular_depreciacion_anual(&temp);
            double tiempo = difftime(time(NULL), temp.fecha_ingreso) / (365.25 * 24 * 3600);

            printf("\n========== ACTIVO ENCONTRADO ==========\n");
            printf("ID:               %s\n",    temp.id);
            printf("Nombre:           %s\n",    temp.nombre);
            printf("Descripcion:      %s\n",    temp.descripcion);
            printf("Costo inicial:    $%.2f\n", temp.costo_inicial);
            printf("Valor residual:   $%.2f\n", temp.valor_residual);
            printf("Valor actual:     $%.2f\n", va);
            printf("Deprec. anual:    $%.2f\n", da);
            printf("Tiempo en uso:    %.2f anios\n", tiempo);
            printf("Vida util:        %d anios\n", temp.vida_util);
            printf("Estado:           %s\n",    estado_texto(temp.estado));
            printf("Fecha de ingreso: %s",       ctime(&temp.fecha_ingreso));
            printf("=======================================\n");
            break;
        }
    }

    if(encontrado == 0)
        printf("\nNo se encontro el activo con ID '%s'.\n", id_buscar);

    fclose(archivo);
}

/* ---- mostrar_inventario ----
   Lista TODOS los activos activos (no dados de baja)
   con un formato visual detallado. */
void mostrar_inventario(){
    ActivoFijo temp;
    int total = 0;
    FILE *archivo = fopen("activos.dat", "rb");

    if(archivo == NULL){
        printf("No hay inventario registrado.\n");
        return;
    }

    printf("\n");
    imprimir_separador('=', 70);
    printf("                   INVENTARIO COMPLETO\n");
    imprimir_separador('=', 70);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(temp.estado == 4) continue; /* No mostrar los dados de baja */

        /* total+1 sirve como numero de fila visual */
        printf("[%d] ID: %-14s | Nombre: %-20s | Estado: %s\n",
               total + 1, temp.id, temp.nombre, estado_texto(temp.estado));
        printf("     Costo: $%-10.2f | Valor actual: $%-10.2f | Vida util: %d anios\n",
               temp.costo_inicial, calcular_valor_actual(&temp), temp.vida_util);
        printf("     Ingreso: %s", ctime(&temp.fecha_ingreso));
        imprimir_separador('-', 70);
        total++;
    }

    printf("Total de activos activos: %d\n", total);
    fclose(archivo);
}

/* ---- Actualizar_activo ----
   Cambia el estado de un activo (Operativo / Mantenimiento /
   Depreciado). Funciona igual que eliminarActivo: busca el
   struct, retrocede con fseek y lo sobreescribe con fwrite. */
void Actualizar_activo(){

    char id_buscar[15];
    ActivoFijo temp;
    int nuevo_estado, encontrado = 0;

    FILE *archivo = fopen("activos.dat", "rb+");
    if(archivo == NULL){
        printf("No existe el inventario registrado.\n");
        return;
    }

    printf("\n========= ACTUALIZAR ESTADO =========\n");
    printf("Ingrese la ID del activo: ");
    scanf("%s", id_buscar);
    limpiar_buffer(); //llamada a funcion de apoyo 

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(strcmp(temp.id, id_buscar) == 0){
            encontrado = 1;
            printf("Activo: %s | Estado actual: %s\n",
                   temp.nombre, estado_texto(temp.estado));
            printf("\nSeleccione nuevo estado:\n");
            printf("  1. Operativo\n  2. Mantenimiento\n  3. Depreciado\n");
            printf("Opcion: ");
            scanf("%d", &nuevo_estado);
            limpiar_buffer();

            if(nuevo_estado < 1 || nuevo_estado > 3){
                printf("[!] Estado no valido.\n");
                break;
            }

            /* Preparamos el mensaje para la bitacora antes de cambiar el estado */
            char accion[60];
            sprintf(accion, "Estado cambiado de '%s' a '%s'",
                    estado_texto(temp.estado), estado_texto(nuevo_estado));

            temp.estado = nuevo_estado;

            /* Retroceder y sobreescribir el struct en el archivo */
            fseek(archivo, -(long)sizeof(ActivoFijo), SEEK_CUR);
            fwrite(&temp, sizeof(ActivoFijo), 1, archivo);

            printf("[OK] Estado actualizado correctamente.\n");
            registrar_evento(id_buscar, "Operador", accion);
            break;
        }
    }

    if(encontrado == 0)
        printf("Activo no encontrado.\n");

    fclose(archivo);
}

/* ---- Monitoreo_de_ciclo_de_vida_del_activo ----
   Para cada activo, calcula que porcentaje de su vida util
   ya se ha consumido y muestra una barra visual de progreso.
   Tambien alerta si el activo esta cerca de su fin de vida. */
void Monitoreo_de_ciclo_de_vida_del_activo(){
    ActivoFijo temp;
    FILE *archivo = fopen("activos.dat", "rb");

    if(archivo == NULL){
        printf("No existe inventario registrado.\n");
        return;
    }

    printf("\n====== MONITOREO DE CICLO DE VIDA ======\n");

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(temp.estado == 4) continue;

        /* difftime devuelve segundos entre dos fechas.
           Dividimos entre segundos en un año para obtener anios. */
        double tiempo = difftime(time(NULL), temp.fecha_ingreso)
                        / (365.25 * 24 * 3600);

        double va = calcular_valor_actual(&temp);
        double porcentaje_vida = (tiempo / temp.vida_util) * 100.0;
        if(porcentaje_vida > 100.0) porcentaje_vida = 100.0;

        printf("\nActivo: %-20s  ID: %s\n", temp.nombre, temp.id);
        printf("  Tiempo en uso:   %.2f / %d anios (%.1f%%)\n",
               tiempo, temp.vida_util, porcentaje_vida);
        printf("  Valor actual:    $%.2f\n", va);

        /* Barra visual de vida RESTANTE:
           20 bloques en total. Calculamos cuantos '#' mostrar
           segun el porcentaje de vida que AUN QUEDA. */
        printf("  Vida restante: [");
        int bloques = (int)((100.0 - porcentaje_vida) / 5.0);
        for(int i = 0; i < 20; i++)
            printf(i < bloques ? "#" : ".");
        printf("] %.1f%% restante\n", 100.0 - porcentaje_vida);

        /* Clasificacion por nivel de alerta */
        if(tiempo >= temp.vida_util)
            printf("  >> CICLO COMPLETADO: Activo totalmente depreciado.\n");
        else if(tiempo >= temp.vida_util * 0.75)
            printf("  >> ALERTA: Proximo al fin de vida util.\n");
        else
            printf("  >> En funcionamiento normal.\n");

        imprimir_separador('-', 55);
    }
    fclose(archivo);
}

/* ---- Historial_Movimiento ----
   Muestra todos los activos registrados (incluyendo bajas)
   con su fecha de ingreso y estado actual.
   Sirve como un historial simple de que entro al sistema. */
void Historial_Movimiento(){
    ActivoFijo temp;
    int movimientos = 0;
    FILE *archivo = fopen("activos.dat", "rb");

    if(archivo == NULL){
        printf("\nNo existe historial disponible.\n");
        return;
    }

    printf("\n========== HISTORIAL DE MOVIMIENTOS ==========\n");

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        printf("Activo: %-20s  ID: %s\n", temp.nombre, temp.id);
        printf("  Fecha de ingreso: %s", ctime(&temp.fecha_ingreso));
        printf("  Estado actual:    %s\n", estado_texto(temp.estado));
        movimientos++;
    }

    printf("\nTotal de registros: %d\n", movimientos);
    fclose(archivo);
}

/* ---- filtrar_por_estado ----
   Permite al operador ver solo los activos que tienen
   un estado especifico */
void filtrar_por_estado(){
    ActivoFijo temp;
    int estado_filtro, encontrados = 0;

    printf("\n===== FILTRAR POR ESTADO =====\n");
    printf("1. Operativo\n2. Mantenimiento\n3. Depreciado\n4. Baja\n");
    printf("Seleccione estado: ");
    scanf("%d", &estado_filtro);
    limpiar_buffer();

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    printf("\nActivos con estado '%s':\n", estado_texto(estado_filtro));
    imprimir_separador('-', 55);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(temp.estado == estado_filtro){
            printf("ID: %-14s | Nombre: %-20s\n", temp.id, temp.nombre);
            printf("  Valor actual: $%.2f | Ingreso: %s",
                   calcular_valor_actual(&temp), ctime(&temp.fecha_ingreso));
            encontrados++;
        }
    }

    if(encontrados == 0)
        printf("[!] No hay activos con ese estado.\n");
    else
        printf("\nTotal encontrados: %d\n", encontrados);

    fclose(archivo);
}


/* ==============================================================
   SECCION 7: FUNCIONES DE MANTENIMIENTO 
   ============================================================== */

/* ---- registrar_mantenimiento ----
   Primero verifica que el ID ingresado corresponda a un
   activo existente y no dado de baja. Luego llena un struct
   RegistroMantenimiento y lo guarda en "mantenimiento.dat". */
void registrar_mantenimiento(){
    RegistroMantenimiento reg;
    char id_activo[15];
    ActivoFijo temp;
    int activo_valido = 0;

    /* Paso 1: verificar que el activo existe */
    FILE *arch_activos = fopen("activos.dat", "rb");
    if(arch_activos == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    printf("\n===== REGISTRAR MANTENIMIENTO =====\n");
    printf("ID del activo a dar mantenimiento: ");
    scanf("%s", id_activo);
    limpiar_buffer();

    while(fread(&temp, sizeof(ActivoFijo), 1, arch_activos) == 1){
        if(strcmp(temp.id, id_activo) == 0 && temp.estado != 4){
            activo_valido = 1;
            printf("[OK] Activo encontrado: %s\n", temp.nombre);
            break;
        }
    }
    fclose(arch_activos);

    if(!activo_valido){
        printf("[!] Activo no encontrado o dado de baja.\n");
        return;
    }

    /* Paso 2: llenar el registro de mantenimiento */
    strncpy(reg.id_activo, id_activo, 14);
    reg.id_activo[14] = '\0'; /* Asegurar terminador de cadena */

    printf("Descripcion del mantenimiento: ");
    fgets(reg.descripcion, 150, stdin);
    reg.descripcion[strcspn(reg.descripcion, "\n")] = 0;

    printf("Costo del mantenimiento: $");
    scanf("%lf", &reg.costo_mantenimiento);
    limpiar_buffer();

    printf("Tipo: 1. Preventivo  2. Correctivo: ");
    scanf("%d", &reg.tipo);
    limpiar_buffer();

    reg.fecha_mantenimiento = time(NULL);

    /* Paso 3: guardar en archivo */
    FILE *arch_mant = fopen("mantenimiento.dat", "ab");
    if(arch_mant != NULL){
        fwrite(&reg, sizeof(RegistroMantenimiento), 1, arch_mant);
        fclose(arch_mant);
        printf("[OK] Mantenimiento registrado correctamente.\n");
        registrar_evento(id_activo, "Mantenimiento",
                         reg.tipo == 1 ? "Mantenimiento preventivo"
                                       : "Mantenimiento correctivo");
    } else {
        printf("[!] Error al guardar el mantenimiento.\n");
    }
}

/* ---- ver_historial_mantenimiento ----
   Lee y muestra todos los registros de "mantenimiento.dat". */
void ver_historial_mantenimiento(){
    RegistroMantenimiento reg;
    int total = 0;

    FILE *archivo = fopen("mantenimiento.dat", "rb");
    if(archivo == NULL){
        printf("\n[!] No hay historial de mantenimiento.\n");
        return;
    }

    printf("\n===== HISTORIAL DE MANTENIMIENTO =====\n");
    imprimir_separador('-', 55);

    while(fread(&reg, sizeof(RegistroMantenimiento), 1, archivo) == 1){
        printf("Activo ID:   %s\n", reg.id_activo);
        printf("Descripcion: %s\n", reg.descripcion);
        printf("Tipo:        %s\n", reg.tipo == 1 ? "Preventivo" : "Correctivo");
        printf("Costo:       $%.2f\n", reg.costo_mantenimiento);
        printf("Fecha:       %s", ctime(&reg.fecha_mantenimiento));
        imprimir_separador('-', 55);
        total++;
    }

    printf("Total de registros de mantenimiento: %d\n", total);
    fclose(archivo);
}

/* ---- activos_en_mantenimiento ----
   Filtra y muestra unicamente los activos cuyo estado
   actual sea 2 (En mantenimiento). */
void activos_en_mantenimiento(){
    ActivoFijo temp;
    int encontrados = 0;

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    printf("\n===== ACTIVOS EN MANTENIMIENTO =====\n");
    imprimir_separador('-', 50);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(temp.estado == 2){
            printf("ID: %-14s | Nombre: %s\n", temp.id, temp.nombre);
            printf("  Costo inicial: $%.2f | Valor actual: $%.2f\n",
                   temp.costo_inicial, calcular_valor_actual(&temp));
            encontrados++;
        }
    }

    if(encontrados == 0)
        printf("No hay activos en mantenimiento actualmente.\n");
    else
        printf("\nTotal en mantenimiento: %d\n", encontrados);

    fclose(archivo);
}


/* ==============================================================
   SECCION 8: FUNCIONES DE ESTADISTICAS
   ============================================================== */

/* ---- estadisticas_generales ----
   Recorre todo el inventario y acumula contadores y sumas.
   Al final imprime un resumen financiero y operativo. */
void estadisticas_generales(){
    ActivoFijo temp;
    /* Inicializamos todos los contadores en 0 */
    int total = 0, operativos = 0, mantenimiento = 0, depreciados = 0, bajas = 0;
    double inversion_total = 0.0, valor_actual_total = 0.0;

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    /* Un solo recorrido acumula todo lo necesario */
    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        total++;
        inversion_total    += temp.costo_inicial;
        valor_actual_total += calcular_valor_actual(&temp);

        if     (temp.estado == 1) operativos++;
        else if(temp.estado == 2) mantenimiento++;
        else if(temp.estado == 3) depreciados++;
        else if(temp.estado == 4) bajas++;
    }
    fclose(archivo);

    printf("\n");
    imprimir_separador('=', 45);
    printf("         ESTADISTICAS GENERALES\n");
    imprimir_separador('=', 45);
    printf("Total de activos registrados: %d\n",   total);
    printf("  - Operativos:               %d\n",   operativos);
    printf("  - En mantenimiento:         %d\n",   mantenimiento);
    printf("  - Depreciados:              %d\n",   depreciados);
    printf("  - Dados de baja:            %d\n",   bajas);
    imprimir_separador('-', 45);
    printf("Inversion total registrada:  $%.2f\n", inversion_total);
    printf("Valor actual del inventario: $%.2f\n", valor_actual_total);
    /* La diferencia entre lo que costaron y lo que valen hoy */
    printf("Perdida por depreciacion:    $%.2f\n", inversion_total - valor_actual_total);
    imprimir_separador('=', 45);
}

/* ---- resumen_depreciacion ----
   Muestra en formato de tabla cuanto pierde de valor cada
   activo por año, y que porcentaje ya se ha depreciado. */
void resumen_depreciacion(){
    ActivoFijo temp;
    FILE *archivo = fopen("activos.dat", "rb");

    if(archivo == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    printf("\n");
    imprimir_separador('-', 65);
    printf("%-15s %-18s %-12s %-12s\n",
           "ID", "Nombre", "Depr./Anio", "% Deprec.");
    imprimir_separador('-', 65);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        if(temp.estado == 4) continue;
        double da = calcular_depreciacion_anual(&temp);
        double va = calcular_valor_actual(&temp);
        /* Porcentaje depreciado = cuanto perdio / costo original * 100 */
        double porcentaje = ((temp.costo_inicial - va) / temp.costo_inicial) * 100.0;

        printf("%-15s %-18s $%-11.2f %.1f%%\n",
               temp.id, temp.nombre, da, porcentaje);
    }

    imprimir_separador('-', 65);
    fclose(archivo);
}

/* ---- activos_proximos_depreciar ----
   Alerta sobre activos que ya consumieron mas del 75% de
   su vida util. Son candidatos a reemplazarse pronto. */
void activos_proximos_depreciar(){
    ActivoFijo temp;
    int encontrados = 0;
    double umbral = 0.75; /* 75% = tres cuartos de vida util consumida */

    FILE *archivo = fopen("activos.dat", "rb");
    if(archivo == NULL){
        printf("[!] No hay inventario registrado.\n");
        return;
    }

    printf("\n===== ACTIVOS PROXIMOS A DEPRECIAR (>75%% vida util) =====\n");
    imprimir_separador('-', 60);

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
        /* Ignorar dados de baja y los que ya estan depreciados */
        if(temp.estado == 4 || temp.estado == 3) continue;

        double tiempo = difftime(time(NULL), temp.fecha_ingreso)
                        / (365.25 * 24 * 3600);
        double porcentaje = tiempo / temp.vida_util;

        if(porcentaje >= umbral){
            printf("ID: %-14s | Nombre: %-18s\n", temp.id, temp.nombre);
            printf("  Vida usada: %.1f%% | Tiempo: %.2f / %d anios\n",
                   porcentaje * 100.0, tiempo, temp.vida_util);
            printf("  Valor actual: $%.2f\n", calcular_valor_actual(&temp));
            imprimir_separador('-', 60);
            encontrados++;
        }
    }

    if(encontrados == 0)
        printf("Ningun activo supera el umbral del 75%% de vida util.\n");
    else
        printf("Total de activos en alerta: %d\n", encontrados);

    fclose(archivo);
}


/* ==============================================================
   SECCION 9: BITACORA DEL SISTEMA
   Registra automaticamente cada accion importante.
   No es visible en el menu normal; solo desde "Ver bitacora".
   ============================================================== */

/* ---- registrar_evento ----
   Esta funcion es llamada internamente por otras funciones
   (no directamente por el usuario) cada vez que ocurre una
   accion importante: alta, baja, cambio de estado, mantenimiento.
   Guarda todo en "bitacora.dat". */
void registrar_evento(const char *id_activo, const char *usuario, const char *accion){
    BitacoraEvento evento;

    /* strncpy copia hasta N caracteres de una cadena a otra,
       evitando desbordamientos de buffer */
    strncpy(evento.id_activo, id_activo, 14); evento.id_activo[14] = '\0';
    strncpy(evento.usuario,   usuario,   29); evento.usuario[29]   = '\0';
    strncpy(evento.accion,    accion,    99); evento.accion[99]    = '\0';
    evento.fecha_hora = time(NULL);

    FILE *bitacora = fopen("bitacora.dat", "ab");
    if(bitacora != NULL){
        fwrite(&evento, sizeof(BitacoraEvento), 1, bitacora);
        fclose(bitacora);
    }
    /* Si no se puede abrir el archivo de bitacora,
       el sistema sigue funcionando sin interrumpir al usuario */
}

/* ---- ver_bitacora ----
   Muestra el historial completo de eventos del sistema.
   Util para auditoria: saber quien hizo que y cuando. */
void ver_bitacora(){
    BitacoraEvento evento;
    int total = 0;

    FILE *archivo = fopen("bitacora.dat", "rb");
    if(archivo == NULL){
        printf("\n[!] No hay eventos en la bitacora.\n");
        return;
    }

    printf("\n");
    imprimir_separador('=', 60);
    printf("              BITACORA DEL SISTEMA\n");
    imprimir_separador('=', 60);

    while(fread(&evento, sizeof(BitacoraEvento), 1, archivo) == 1){
        printf("Activo: %-12s  |  Usuario: %-15s\n",
               evento.id_activo, evento.usuario);
        printf("  Accion: %s\n", evento.accion);
        printf("  Fecha:  %s", ctime(&evento.fecha_hora));
        imprimir_separador('-', 60);
        total++;
    }

    printf("Total de eventos registrados: %d\n", total);
    fclose(archivo);
}


/* ==============================================================
   SECCION 10: FUNCIONES UTILITARIAS
   Son herramientas de apoyo usadas por muchas otras funciones.
   ============================================================== */

/* ---- limpiar_buffer ----
   Despues de scanf, el caracter '\n' (Enter) queda en el
   buffer de entrada. Si no se limpia, la siguiente lectura
   con fgets lo leria de inmediato sin esperar al usuario.
   Este ciclo consume todos los caracteres hasta el '\n'. */
void limpiar_buffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

/* ---- pausar ----
   Detiene la ejecucion hasta que el usuario presione Enter.
   Util para que el usuario pueda leer un resultado antes
   de que el menu vuelva a aparecer. */
void pausar(){
    printf("\nPresione Enter para continuar...");
    getchar();
}

/* ---- estado_texto ----
   Convierte el numero entero del estado en su texto legible.
   En lugar de escribir el switch en cada funcion que necesita
   mostrar el estado, se llama a esta funcion una sola vez.
   Devuelve un puntero a un literal de cadena (string constante). */
const char* estado_texto(int estado){
    switch(estado){
        case 1:  return "Operativo";
        case 2:  return "Mantenimiento";
        case 3:  return "Depreciado";
        case 4:  return "Baja";
        default: return "Desconocido";
    }
}

/* ---- calcular_valor_actual ----
   Calcula cuanto vale hoy un activo usando depreciacion
   lineal (metodo de linea recta):
     Depreciacion por año = (Costo - Valor residual) / Vida util
     Valor actual = Costo - (Depreciacion anual * Tiempo transcurrido)
   Si el valor calculado cae por debajo del valor residual,
   se usa el valor residual como piso minimo.
   Recibe un puntero a ActivoFijo para no copiar el struct completo. */
double calcular_valor_actual(ActivoFijo *a){
    /* Calcular anios transcurridos desde la fecha de ingreso */
    double tiempo = difftime(time(NULL), a->fecha_ingreso)
                    / (365.25 * 24 * 3600);

    double depreciacion = ((a->costo_inicial - a->valor_residual)
                           / a->vida_util) * tiempo;

    double valor = a->costo_inicial - depreciacion;

    /* Operador ternario: si valor < residual, usar residual */
    return valor < a->valor_residual ? a->valor_residual : valor;
}

/* ---- calcular_depreciacion_anual ----
   Devuelve cuanto pierde de valor el activo cada año.
   Formula: (Costo inicial - Valor residual) / Vida util
   Si vida_util es 0 se devuelve 0 para evitar division por cero. */
double calcular_depreciacion_anual(ActivoFijo *a){
    if(a->vida_util == 0) return 0.0;
    return (a->costo_inicial - a->valor_residual) / a->vida_util;
}

/* ---- imprimir_separador ----
   Imprime una linea de N veces el caracter 'c' seguida de salto.
   Ejemplo: imprimir_separador('-', 40) imprime 40 guiones.
   Sirve para crear los bordes visuales de los menus y tablas. */
void imprimir_separador(char c, int n){
    for(int i = 0; i < n; i++) printf("%c", c);
    printf("\n");
}
