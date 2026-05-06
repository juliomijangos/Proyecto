#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

// ========================= ESTRUCTURAS AUXILIARES =========================
struct Fecha {
    int dia, mes, anio;
};

// ========================= CLASE ACTIVO =========================
class Activo {
private:
    int id;
    char nombre[50];
    char descripcion[100];
    double costo_inicial;
    double valor_residual;
    int vida_util;          // en años
    Fecha fecha_adquisicion;
    char estado[20];        // "Operativo", "Mantenimiento", "Depreciado"
    char ubicacion[50];

public:
    // Constructor por defecto
    Activo() : id(0), costo_inicial(0), valor_residual(0), vida_util(0) {
        strcpy(nombre, "");
        strcpy(descripcion, "");
        strcpy(estado, "Operativo");
        strcpy(ubicacion, "");
        fecha_adquisicion = {0, 0, 0};
    }

    // Setters
    void setId(int i) { id = i; }
    void setNombre(const char* n) { strcpy(nombre, n); }
    void setDescripcion(const char* d) { strcpy(descripcion, d); }
    void setCostoInicial(double c) { costo_inicial = c; }
    void setValorResidual(double v) { valor_residual = v; }
    void setVidaUtil(int v) { vida_util = v; }
    void setFecha(int d, int m, int a) { fecha_adquisicion = {d, m, a}; }
    void setEstado(const char* e) { strcpy(estado, e); }
    void setUbicacion(const char* u) { strcpy(ubicacion, u); }

    // Getters
    int getId() const { return id; }
    const char* getNombre() const { return nombre; }
    const char* getDescripcion() const { return descripcion; }
    double getCostoInicial() const { return costo_inicial; }
    double getValorResidual() const { return valor_residual; }
    int getVidaUtil() const { return vida_util; }
    Fecha getFecha() const { return fecha_adquisicion; }
    const char* getEstado() const { return estado; }
    const char* getUbicacion() const { return ubicacion; }

    // Cálculo de depreciación lineal
    double calcularValorActual(time_t fecha_hoy) const {
        if (vida_util <= 0) return costo_inicial;

        // Obtener fecha actual
        struct tm* tm_hoy = localtime(&fecha_hoy);
        Fecha hoy = {tm_hoy->tm_mday, tm_hoy->tm_mon + 1, tm_hoy->tm_year + 1900};

        // Calcular años transcurridos (aproximado)
        int anios_transcurridos = hoy.anio - fecha_adquisicion.anio;
        if (hoy.mes < fecha_adquisicion.mes) anios_transcurridos--;
        else if (hoy.mes == fecha_adquisicion.mes && hoy.dia < fecha_adquisicion.dia) anios_transcurridos--;

        if (anios_transcurridos <= 0) return costo_inicial;

        double depreciacion_anual = (costo_inicial - valor_residual) / vida_util;
        double valor_actual = costo_inicial - (depreciacion_anual * anios_transcurridos);

        if (valor_actual < valor_residual) valor_actual = valor_residual;
        return valor_actual;
    }

    // Mostrar información del activo
    void mostrar() const {
        cout << "ID: " << id << endl;
        cout << "Nombre: " << nombre << endl;
        cout << "Descripcion: " << descripcion << endl;
        cout << "Costo inicial: $" << fixed << setprecision(2) << costo_inicial << endl;
        cout << "Valor residual: $" << valor_residual << endl;
        cout << "Vida util: " << vida_util << " anios" << endl;
        cout << "Fecha adquisicion: " << fecha_adquisicion.dia << "/" << fecha_adquisicion.mes << "/" << fecha_adquisicion.anio << endl;
        cout << "Estado: " << estado << endl;
        cout << "Ubicacion: " << ubicacion << endl;

        time_t t = time(nullptr);
        cout << "Valor actual (depreciado): $" << calcularValorActual(t) << endl;
        cout << "------------------------" << endl;
    }

    // Guardar en archivo binario
    void guardar(ofstream& out) const {
        out.write(reinterpret_cast<const char*>(this), sizeof(Activo));
    }

    // Cargar desde archivo binario
    void cargar(ifstream& in) {
        in.read(reinterpret_cast<char*>(this), sizeof(Activo));
    }
};

// ========================= CLASE GESTOR DE ACTIVOS =========================
class GestorActivos {
private:
    vector<Activo> activos;
    const char* archivo_datos = "activos.dat";
    const char* archivo_log = "movimientos.log";

    void escribirLog(const string& mensaje) {
        ofstream log(archivo_log, ios::app);
        if (log.is_open()) {
            time_t t = time(nullptr);
            struct tm* tm_info = localtime(&t);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
            log << buffer << " - " << mensaje << endl;
            log.close();
        }
    }

public:
    GestorActivos() {
        cargarDesdeArchivo();
    }

    ~GestorActivos() {
        guardarEnArchivo();
    }

    void cargarDesdeArchivo() {
        ifstream in(archivo_datos, ios::binary);
        if (!in.is_open()) return;

        Activo a;
        while (in.peek() != EOF) {
            a.cargar(in);
            if (in.good())
                activos.push_back(a);
        }
        in.close();
    }

    void guardarEnArchivo() {
        ofstream out(archivo_datos, ios::binary | ios::trunc);
        for (const auto& a : activos) {
            a.guardar(out);
        }
        out.close();
    }

    // Registrar nuevo activo (solo admin)
    bool registrarActivo(int id, const char* nombre, const char* descripcion,
                         double costo, double valor_res, int vida,
                         int dia, int mes, int anio, const char* ubic) {
        // Validar ID único
        for (const auto& a : activos) {
            if (a.getId() == id) {
                cout << "Error: ID ya existe." << endl;
                return false;
            }
        }
        // Validar costos positivos
        if (costo <= 0 || valor_res < 0 || valor_res > costo) {
            cout << "Error: Costo invalido o valor residual mayor al costo." << endl;
            return false;
        }

        Activo nuevo;
        nuevo.setId(id);
        nuevo.setNombre(nombre);
        nuevo.setDescripcion(descripcion);
        nuevo.setCostoInicial(costo);
        nuevo.setValorResidual(valor_res);
        nuevo.setVidaUtil(vida);
        nuevo.setFecha(dia, mes, anio);
        nuevo.setEstado("Operativo");
        nuevo.setUbicacion(ubic);

        activos.push_back(nuevo);
        guardarEnArchivo();

        string logMsg = "Registrado activo ID " + to_string(id) + " - " + nombre;
        escribirLog(logMsg);
        return true;
    }

    // Consultar activo por ID o nombre
    Activo* consultarActivo(const string& criterio) {
        for (auto& a : activos) {
            if (to_string(a.getId()) == criterio || string(a.getNombre()) == criterio) {
                return &a;
            }
        }
        return nullptr;
    }

    // Mostrar todos los activos
    void mostrarInventario() const {
        if (activos.empty()) {
            cout << "Inventario vacio." << endl;
            return;
        }
        time_t t = time(nullptr);
        for (const auto& a : activos) {
            a.mostrar();
            cout << "Valor actual con depreciacion: $" << fixed << setprecision(2) << a.calcularValorActual(t) << endl;
            cout << "------------------------" << endl;
        }
    }

    // Actualizar estado de activo (operador o admin)
    bool actualizarEstado(int id, const char* nuevo_estado) {
        Activo* a = consultarActivo(to_string(id));
        if (!a) {
            cout << "Activo no encontrado." << endl;
            return false;
        }
        a->setEstado(nuevo_estado);
        guardarEnArchivo();

        string logMsg = "Actualizado estado Activo ID " + to_string(id) + " a " + nuevo_estado;
        escribirLog(logMsg);
        return true;
    }

    // Modificar datos financieros (solo admin)
    bool modificarParametros(int id, double nuevo_valor_residual, int nueva_vida_util) {
        Activo* a = consultarActivo(to_string(id));
        if (!a) {
            cout << "Activo no encontrado." << endl;
            return false;
        }
        if (nuevo_valor_residual < 0 || nuevo_valor_residual > a->getCostoInicial()) {
            cout << "Valor residual invalido." << endl;
            return false;
        }
        a->setValorResidual(nuevo_valor_residual);
        a->setVidaUtil(nueva_vida_util);
        guardarEnArchivo();

        string logMsg = "Modificados parametros financieros Activo ID " + to_string(id);
        escribirLog(logMsg);
        return true;
    }

    // Reporte financiero: inversión total y valor actual neto
    void generarReporteFinanciero() const {
        if (activos.empty()) {
            cout << "No hay activos para generar reporte." << endl;
            return;
        }

        double inversion_total = 0;
        double patrimonio_neto = 0;
        time_t t = time(nullptr);

        for (const auto& a : activos) {
            inversion_total += a.getCostoInicial();
            patrimonio_neto += a.calcularValorActual(t);
        }

        cout << "\n========== REPORTE FINANCIERO ==========" << endl;
        cout << "Total inversion inicial: $" << fixed << setprecision(2) << inversion_total << endl;
        cout << "Valor actual del patrimonio neto: $" << patrimonio_neto << endl;
        cout << "Depreciacion acumulada total: $" << (inversion_total - patrimonio_neto) << endl;
        cout << "========================================\n" << endl;
    }

    // Monitoreo de activos próximos a depreciarse totalmente
    void monitoreoCicloVida() const {
        time_t t = time(nullptr);
        bool hay_criticos = false;
        cout << "\n=== ACTIVOS CERCANOS A DEPRECIACION TOTAL ===" << endl;
        for (const auto& a : activos) {
            double valor_actual = a.calcularValorActual(t);
            if (valor_actual <= a.getValorResidual() * 1.1 && valor_actual > a.getValorResidual()) {
                cout << "ID: " << a.getId() << " - " << a.getNombre() << " - Valor actual cercano a residual." << endl;
                hay_criticos = true;
            }
        }
        if (!hay_criticos) cout << "Todos los activos estan en condiciones saludables." << endl;
    }

    void mostrarLogs() const {
        ifstream log(archivo_log);
        if (!log.is_open()) {
            cout << "No hay registros de movimientos." << endl;
            return;
        }
        string linea;
        cout << "\n=== HISTORIAL DE MOVIMIENTOS ===" << endl;
        while (getline(log, linea)) {
            cout << linea << endl;
        }
        log.close();
    }
};

// ========================= INTERFAZ DE USUARIO =========================
void menuAdmin(GestorActivos& gestor) {
    int opcion;
    do {
        cout << "\n===== MENU ADMINISTRADOR =====" << endl;
        cout << "1. Registrar nuevo activo" << endl;
        cout << "2. Consultar activo por ID/Nombre" << endl;
        cout << "3. Mostrar todo el inventario" << endl;
        cout << "4. Actualizar estado de activo" << endl;
        cout << "5. Modificar parametros financieros" << endl;
        cout << "6. Generar reporte financiero" << endl;
        cout << "7. Monitoreo de ciclo de vida" << endl;
        cout << "8. Ver historial de movimientos" << endl;
        cout << "9. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore();

        switch(opcion) {
            case 1: {
                int id, dia, mes, anio, vida;
                char nombre[50], desc[100], ubic[50];
                double costo, valor_res;
                cout << "ID: "; cin >> id;
                cin.ignore();
                cout << "Nombre: "; cin.getline(nombre, 50);
                cout << "Descripcion: "; cin.getline(desc, 100);
                cout << "Costo inicial: "; cin >> costo;
                cout << "Valor residual: "; cin >> valor_res;
                cout << "Vida util (años): "; cin >> vida;
                cout << "Fecha adquisicion (dia mes anio): "; cin >> dia >> mes >> anio;
                cin.ignore();
                cout << "Ubicacion: "; cin.getline(ubic, 50);
                gestor.registrarActivo(id, nombre, desc, costo, valor_res, vida, dia, mes, anio, ubic);
                break;
            }
            case 2: {
                string criterio;
                cout << "Ingrese ID o Nombre: ";
                cin.ignore();
                getline(cin, criterio);
                Activo* a = gestor.consultarActivo(criterio);
                if (a) a->mostrar();
                else cout << "Activo no encontrado." << endl;
                break;
            }
            case 3:
                gestor.mostrarInventario();
                break;
            case 4: {
                int id;
                char estado[20];
                cout << "ID activo: "; cin >> id;
                cout << "Nuevo estado (Operativo/Mantenimiento/Depreciado): ";
                cin >> estado;
                gestor.actualizarEstado(id, estado);
                break;
            }
            case 5: {
                int id, nueva_vida;
                double nuevo_valor_res;
                cout << "ID activo: "; cin >> id;
                cout << "Nuevo valor residual: "; cin >> nuevo_valor_res;
                cout << "Nueva vida util (años): "; cin >> nueva_vida;
                gestor.modificarParametros(id, nuevo_valor_res, nueva_vida);
                break;
            }
            case 6:
                gestor.generarReporteFinanciero();
                break;
            case 7:
                gestor.monitoreoCicloVida();
                break;
            case 8:
                gestor.mostrarLogs();
                break;
            case 9:
                cout << "Saliendo del modulo administrador..." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
        }
    } while(opcion != 9);
}

void menuOperador(GestorActivos& gestor) {
    int opcion;
    do {
        cout << "\n===== MENU OPERADOR DE ALMACEN =====" << endl;
        cout << "1. Consultar activo por ID/Nombre" << endl;
        cout << "2. Mostrar todo el inventario" << endl;
        cout << "3. Actualizar estado de activo" << endl;
        cout << "4. Monitoreo de ciclo de vida" << endl;
        cout << "5. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore();

        switch(opcion) {
            case 1: {
                string criterio;
                cout << "Ingrese ID o Nombre: ";
                getline(cin, criterio);
                Activo* a = gestor.consultarActivo(criterio);
                if (a) a->mostrar();
                else cout << "Activo no encontrado." << endl;
                break;
            }
            case 2:
                gestor.mostrarInventario();
                break;
            case 3: {
                int id;
                char estado[20];
                cout << "ID activo: "; cin >> id;
                cout << "Nuevo estado (Operativo/Mantenimiento/Depreciado): ";
                cin >> estado;
                gestor.actualizarEstado(id, estado);
                break;
            }
            case 4:
                gestor.monitoreoCicloVida();
                break;
            case 5:
                cout << "Saliendo del modulo operador..." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
        }
    } while(opcion != 5);
}

// ========================= MAIN =========================
int main() {
    GestorActivos gestor;
    int rol;

    cout << "=== SISTEMA DE GESTION DE ACTIVOS FIJOS (SISTEMAS GAF) ===" << endl;
    cout << "Seleccione su rol:" << endl;
    cout << "1. Administrador de Activos" << endl;
    cout << "2. Operador de Almacen" << endl;
    cout << "Opcion: ";
    cin >> rol;

    if (rol == 1) {
        menuAdmin(gestor);
    } else if (rol == 2) {
        menuOperador(gestor);
    } else {
        cout << "Rol no valido." << endl;
    }

    return 0;
}
