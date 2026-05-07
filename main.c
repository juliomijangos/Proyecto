C. 29C 935-937, 97314 Mérida, Yuc.

#include <stdio.h>
#include <time.h>

// ========================= ESTRUCTURAS =========================
typedef struct {
    int dia, mes, anio;
} Fecha;

typedef struct {
    int id;
    char nombre[50];
    char descripcion[100];
    double costo_inicial;
    double valor_residual;
    int vida_util;
    Fecha fecha_adquisicion;
    char estado[20];
    char ubicacion[50];
} Activo;

// ========================= FUNCIONES AUXILIARES =========================

int mi_strlen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void mi_strcpy(char* dest, const char* src) {
    int i = 0;
    while ((dest[i] = src[i]) != '\0') i++;
}

int mi_strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

// Convierte int a string, retorna longitud
int int_a_str(int n, char* buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
    char tmp[20];
    int i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    if (neg) tmp[i++] = '-';
    int len = i;
    for (int j = 0; j < len; j++) buf[j] = tmp[len - 1 - j];
    buf[len] = '\0';
    return len;
}

// ========================= ACTIVOS: arreglo dinámico simple =========================
#define MAX_ACTIVOS 200

Activo activos[MAX_ACTIVOS];
int num_activos = 0;

const char* ARCHIVO_DATOS = "activos.dat";
const char* ARCHIVO_LOG   = "movimientos.log";

// ========================= FUNCIONES DE CÁLCULO =========================
double calcularValorActual(const Activo* a, time_t fecha_hoy) {
    if (a->vida_util <= 0) return a->costo_inicial;

    struct tm* tm_hoy = localtime(&fecha_hoy);
    int hoy_dia  = tm_hoy->tm_mday;
    int hoy_mes  = tm_hoy->tm_mon + 1;
    int hoy_anio = tm_hoy->tm_year + 1900;

    int anios = hoy_anio - a->fecha_adquisicion.anio;
    if (hoy_mes < a->fecha_adquisicion.mes) anios--;
    else if (hoy_mes == a->fecha_adquisicion.mes && hoy_dia < a->fecha_adquisicion.dia) anios--;

    if (anios <= 0) return a->costo_inicial;

    double dep_anual = (a->costo_inicial - a->valor_residual) / a->vida_util;
    double valor = a->costo_inicial - (dep_anual * anios);
    return (valor < a->valor_residual) ? a->valor_residual : valor;
}

// ========================= LOG =========================
void escribirLog(const char* mensaje) {
    FILE* log = fopen(ARCHIVO_LOG, "a");
    if (!log) return;

    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char buffer[80];
    // Formato manual: AAAA-MM-DD HH:MM:SS
    // usamos strftime que es parte de <time.h>
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(log, "%s - %s\n", buffer, mensaje);
    fclose(log);
}

// ========================= ARCHIVO BINARIO =========================
void cargarDesdeArchivo() {
    FILE* in = fopen(ARCHIVO_DATOS, "rb");
    if (!in) return;
    num_activos = 0;
    while (num_activos < MAX_ACTIVOS &&
           fread(&activos[num_activos], sizeof(Activo), 1, in) == 1) {
        num_activos++;
    }
    fclose(in);
}

void guardarEnArchivo() {
    FILE* out = fopen(ARCHIVO_DATOS, "wb");
    if (!out) return;
    for (int i = 0; i < num_activos; i++) {
        fwrite(&activos[i], sizeof(Activo), 1, out);
    }
    fclose(out);
}

// ========================= MOSTRAR ACTIVO =========================
void mostrarActivo(const Activo* a) {
    time_t t = time(NULL);
    printf("ID: %d\n", a->id);
    printf("Nombre: %s\n", a->nombre);
    printf("Descripcion: %s\n", a->descripcion);
    printf("Costo inicial: $%.2f\n", a->costo_inicial);
    printf("Valor residual: $%.2f\n", a->valor_residual);
    printf("Vida util: %d anios\n", a->vida_util);
    printf("Fecha adquisicion: %d/%d/%d\n",
           a->fecha_adquisicion.dia,
           a->fecha_adquisicion.mes,
           a->fecha_adquisicion.anio);
    printf("Estado: %s\n", a->estado);
    printf("Ubicacion: %s\n", a->ubicacion);
    printf("Valor actual (depreciado): $%.2f\n", calcularValorActual(a, t));
    printf("------------------------\n");
}

// ========================= OPERACIONES =========================
int registrarActivo(int id, const char* nombre, const char* descripcion,
                    double costo, double valor_res, int vida,
                    int dia, int mes, int anio, const char* ubic) {
    for (int i = 0; i < num_activos; i++) {
        if (activos[i].id == id) {
            printf("Error: ID ya existe.\n");
            return 0;
        }
    }
    if (costo <= 0 || valor_res < 0 || valor_res > costo) {
        printf("Error: Costo invalido o valor residual mayor al costo.\n");
        return 0;
    }
    if (num_activos >= MAX_ACTIVOS) {
        printf("Error: Capacidad maxima alcanzada.\n");
        return 0;
    }

    Activo* nuevo = &activos[num_activos];
    nuevo->id = id;
    mi_strcpy(nuevo->nombre, nombre);
    mi_strcpy(nuevo->descripcion, descripcion);
    nuevo->costo_inicial  = costo;
    nuevo->valor_residual = valor_res;
    nuevo->vida_util      = vida;
    nuevo->fecha_adquisicion.dia  = dia;
    nuevo->fecha_adquisicion.mes  = mes;
    nuevo->fecha_adquisicion.anio = anio;
    mi_strcpy(nuevo->estado, "Operativo");
    mi_strcpy(nuevo->ubicacion, ubic);
    num_activos++;

    guardarEnArchivo();

    char logMsg[120] = "Registrado activo ID ";
    char id_str[20];
    int_a_str(id, id_str);
    // concatenar manualmente
    int pos = mi_strlen(logMsg);
    mi_strcpy(logMsg + pos, id_str);
    pos = mi_strlen(logMsg);
    logMsg[pos++] = ' '; logMsg[pos++] = '-'; logMsg[pos++] = ' '; logMsg[pos] = '\0';
    mi_strcpy(logMsg + pos + 1, nombre);
    escribirLog(logMsg);
    return 1;
}

Activo* consultarActivo(const char* criterio) {
    char id_str[20];
    for (int i = 0; i < num_activos; i++) {
        int_a_str(activos[i].id, id_str);
        if (mi_strcmp(id_str, criterio) == 0 ||
            mi_strcmp(activos[i].nombre, criterio) == 0) {
            return &activos[i];
        }
    }
    return NULL;
}

void mostrarInventario() {
    if (num_activos == 0) { printf("Inventario vacio.\n"); return; }
    time_t t = time(NULL);
    for (int i = 0; i < num_activos; i++) {
        mostrarActivo(&activos[i]);
        printf("Valor actual con depreciacion: $%.2f\n", calcularValorActual(&activos[i], t));
        printf("------------------------\n");
    }
}

int actualizarEstado(int id, const char* nuevo_estado) {
    char id_str[20];
    int_a_str(id, id_str);
    Activo* a = consultarActivo(id_str);
    if (!a) { printf("Activo no encontrado.\n"); return 0; }
    mi_strcpy(a->estado, nuevo_estado);
    guardarEnArchivo();

    char logMsg[80] = "Actualizado estado Activo ID ";
    int pos = mi_strlen(logMsg);
    mi_strcpy(logMsg + pos, id_str);
    pos = mi_strlen(logMsg);
    logMsg[pos++] = ' '; logMsg[pos++] = 'a'; logMsg[pos++] = ' '; logMsg[pos] = '\0';
    mi_strcpy(logMsg + pos, nuevo_estado);
    escribirLog(logMsg);
    return 1;
}

int modificarParametros(int id, double nuevo_valor_res, int nueva_vida) {
    char id_str[20];
    int_a_str(id, id_str);
    Activo* a = consultarActivo(id_str);
    if (!a) { printf("Activo no encontrado.\n"); return 0; }
    if (nuevo_valor_res < 0 || nuevo_valor_res > a->costo_inicial) {
        printf("Valor residual invalido.\n");
        return 0;
    }
    a->valor_residual = nuevo_valor_res;
    a->vida_util = nueva_vida;
    guardarEnArchivo();

    char logMsg[80] = "Modificados parametros financieros Activo ID ";
    int pos = mi_strlen(logMsg);
    mi_strcpy(logMsg + pos, id_str);
    escribirLog(logMsg);
    return 1;
}

void generarReporteFinanciero() {
    if (num_activos == 0) { printf("No hay activos para generar reporte.\n"); return; }
    double inversion_total = 0, patrimonio_neto = 0;
    time_t t = time(NULL);
    for (int i = 0; i < num_activos; i++) {
        inversion_total  += activos[i].costo_inicial;
        patrimonio_neto  += calcularValorActual(&activos[i], t);
    }
    printf("\n========== REPORTE FINANCIERO ==========\n");
    printf("Total inversion inicial:         $%.2f\n", inversion_total);
    printf("Valor actual del patrimonio neto: $%.2f\n", patrimonio_neto);
    printf("Depreciacion acumulada total:    $%.2f\n", inversion_total - patrimonio_neto);
    printf("========================================\n\n");
}

void monitoreoCicloVida() {
    time_t t = time(NULL);
    int hay_criticos = 0;
    printf("\n=== ACTIVOS CERCANOS A DEPRECIACION TOTAL ===\n");
    for (int i = 0; i < num_activos; i++) {
        double va = calcularValorActual(&activos[i], t);
        double vr = activos[i].valor_residual;
        if (va <= vr * 1.1 && va > vr) {
            printf("ID: %d - %s - Valor actual cercano a residual.\n",
                   activos[i].id, activos[i].nombre);
            hay_criticos = 1;
        }
    }
    if (!hay_criticos) printf("Todos los activos estan en condiciones saludables.\n");
}

void mostrarLogs() {
    FILE* log = fopen(ARCHIVO_LOG, "r");
    if (!log) { printf("No hay registros de movimientos.\n"); return; }
    char linea[256];
    printf("\n=== HISTORIAL DE MOVIMIENTOS ===\n");
    while (fgets(linea, sizeof(linea), log)) {
        printf("%s", linea);
    }
    fclose(log);
}

// ========================= MENUS =========================
void menuAdmin() {
    int opcion;
    do {
        printf("\n===== MENU ADMINISTRADOR =====\n");
        printf("1. Registrar nuevo activo\n");
        printf("2. Consultar activo por ID/Nombre\n");
        printf("3. Mostrar todo el inventario\n");
        printf("4. Actualizar estado de activo\n");
        printf("5. Modificar parametros financieros\n");
        printf("6. Generar reporte financiero\n");
        printf("7. Monitoreo de ciclo de vida\n");
        printf("8. Ver historial de movimientos\n");
        printf("9. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: {
                int id, dia, mes, anio, vida;
                char nombre[50], desc[100], ubic[50];
                double costo, valor_res;
                printf("ID: "); scanf("%d", &id); getchar();
                printf("Nombre: "); fgets(nombre, 50, stdin); nombre[mi_strlen(nombre)-1] = '\0';
                printf("Descripcion: "); fgets(desc, 100, stdin); desc[mi_strlen(desc)-1] = '\0';
                printf("Costo inicial: "); scanf("%lf", &costo);
                printf("Valor residual: "); scanf("%lf", &valor_res);
                printf("Vida util (anios): "); scanf("%d", &vida);
                printf("Fecha adquisicion (dia mes anio): "); scanf("%d %d %d", &dia, &mes, &anio);
                getchar();
                printf("Ubicacion: "); fgets(ubic, 50, stdin); ubic[mi_strlen(ubic)-1] = '\0';
                registrarActivo(id, nombre, desc, costo, valor_res, vida, dia, mes, anio, ubic);
                break;
            }
            case 2: {
                char criterio[50];
                printf("Ingrese ID o Nombre: ");
                fgets(criterio, 50, stdin);
                criterio[mi_strlen(criterio)-1] = '\0';
                Activo* a = consultarActivo(criterio);
                if (a) mostrarActivo(a);
                else printf("Activo no encontrado.\n");
                break;
            }
            case 3: mostrarInventario(); break;
            case 4: {
                int id; char estado[20];
                printf("ID activo: "); scanf("%d", &id);
                printf("Nuevo estado (Operativo/Mantenimiento/Depreciado): ");
                scanf("%s", estado);
                actualizarEstado(id, estado);
                break;
            }
            case 5: {
                int id, nueva_vida; double nuevo_val_res;
                printf("ID activo: "); scanf("%d", &id);
                printf("Nuevo valor residual: "); scanf("%lf", &nuevo_val_res);
                printf("Nueva vida util (anios): "); scanf("%d", &nueva_vida);
                modificarParametros(id, nuevo_val_res, nueva_vida);
                break;
            }
            case 6: generarReporteFinanciero(); break;
            case 7: monitoreoCicloVida(); break;
            case 8: mostrarLogs(); break;
            case 9: printf("Saliendo del modulo administrador...\n"); break;
            default: printf("Opcion invalida.\n");
        }
    } while (opcion != 9);
}

void menuOperador() {
    int opcion;
    do {
        printf("\n===== MENU OPERADOR DE ALMACEN =====\n");
        printf("1. Consultar activo por ID/Nombre\n");
        printf("2. Mostrar todo el inventario\n");
        printf("3. Actualizar estado de activo\n");
        printf("4. Monitoreo de ciclo de vida\n");
        printf("5. Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: {
                char criterio[50];
                printf("Ingrese ID o Nombre: ");
                fgets(criterio, 50, stdin);
                criterio[mi_strlen(criterio)-1] = '\0';
                Activo* a = consultarActivo(criterio);
                if (a) mostrarActivo(a);
                else printf("Activo no encontrado.\n");
                break;
            }
            case 2: mostrarInventario(); break;
            case 3: {
                int id; char estado[20];
                printf("ID activo: "); scanf("%d", &id);
                printf("Nuevo estado (Operativo/Mantenimiento/Depreciado): ");
                scanf("%s", estado);
                actualizarEstado(id, estado);
                break;
            }
            case 4: monitoreoCicloVida(); break;
            case 5: printf("Saliendo del modulo operador...\n"); break;
            default: printf("Opcion invalida.\n");
        }
    } while (opcion != 5);
}

// ========================= MAIN =========================
int main() {
    cargarDesdeArchivo();

    int rol;
    printf("=== SISTEMA DE GESTION DE ACTIVOS FIJOS (SISTEMAS GAF) ===\n");
    printf("Seleccione su rol:\n");
    printf("1. Administrador de Activos\n");
    printf("2. Operador de Almacen\n");
    printf("Opcion: ");
    scanf("%d", &rol);

    if (rol == 1) menuAdmin();
    else if (rol == 2) menuOperador();
    else printf("Rol no valido.\n");

    guardarEnArchivo();
    return 0;
}
