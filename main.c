/*DOCUMENTACION DEL CODIGO:
Nombre:  Proyecto-Sistemas SAGA
Compilador:  Dev C++
Programadores:   Julio Cesar  Mijangos Irineo
                 Anna Fernanda Orozco Pozos
                 Daniel Salcedo Miranda
                 ELian Guillermo Pereira Canul
                 Perla Noemi Canche Uicab
Fecha de registro:  16 de mayo del 2026
Version: 1.0

*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

typedef struct {

    char id[15];
    char nombre[50];
    char descripcion[100];
    double costo_inicial;
    double valor_residual;
    int vida_util;
    int estado; //de distintos tipos 1: Operativo, 2:Mantenimiento, 3: Depreciado
    time_t fecha_ingreso;

}ActivoFijo;


//prototipo de funciones

//funcion para los ejecutadores del programa
void menu_principal();

//funciones para el administrador
void menu_administrador();
void menu_Operador_Almacen();
void altaActivo();
void generarReporte();
void limpiar_buffer();

//funciones para el operador de almacen
void consultar_activo();
void mostrar_inventario();
void Actualizar_activo();
void Monitoreo_de_ciclo_de_vida_del_activo();
void Historial_Movimiento();

//añado nueva parte para el operador de almacen
void eliminarActivo();
void reporte_de_texto();


int main(){

    menu_principal();

    return 0;
}

void menu_principal(){

	int opcion();
	do{
		printf("\n=========================================\n");
		printf("|| Sistema de gestion de activos fijos ||\n");
		printf("========================================");
		printf("|| 1.- Modo administrador              ||");
		printf("|| 2.- Operador de almacen             ||");
		printf("|| 3.- Salir                           ||");
		printf("========================================");
		printf("Seleccione una opcion: ");

		if(scanf("%d", &opcion) != 1){
			limpiar_buffer();
			continue;
}
        switch (opcion){
            case 1: menu_administrador();
                break;
            case 2: menu_Operador_Almacen();
                break;
            case 3:
                printf("Saliendo del sistema...\n");
                break;
            default: 
                printf("Opcion invalida. \n");
        }
    } while ( opcion != 3);
}

//funcion para mostral el menu del administrador
void menu_administrador(){
  
  int opcion;
  printf("\n+----------------------------------+\n");
  printf("|      Menu de administrador       |\n");
  printf("+----------------------------------+\n");
  printf("| 1.- Registrar nuevo activo       |");
  printf("| 2.- Generar reporte de inversion |");
  printf("| 3.- Dar de baja un activo        |");
  printf("| 4.- Generar reporte de texto     |");
  printf("| 5.- Volver                       |");
  printf("Seleccione una opcion: ");
  scanf("%d", &opcion);
  limpiar_buffer();

  switch (opcion){
	  case 1: altaActivo();
	  break;
	  case 2: generarReporte();
	  break;
	  case 3: eliminarActivo();
	  break;
	  case 4: reporte_de_texto();
	  break;
	  case 5: return;
	  default:
	  printf("\n[!] Opcion no valida.\n");
  }
}

//funcion para el operador de almacen
void menu_Operador_Almacen(){

  int ops;
  do{
  printf("\n+---------------------------------------|\n");
  printf("|            Menu de Operador           |\n");
  printf("+---------------------------------------+\n");
  printf("|  1. Consultar activo por ID/Nombre    |\n");
  printf("|  2. Mostrar todo el inventario        |\n");
  printf("|  3. Actualizar estado de activo       |\n");
  printf("|  4. Monitoreo de ciclo de vida        |\n");
  printf("|  5. Ver historial de movimientos      |\n");
  printf("|  6. Volver                            |\n");
  printf("+---------------------------------------+\n");
  printf("Seleccione una opcion: ");
  scanf("%d", &ops);

  switch (ops){
    case 1:
        consultar_activo();
        break;
    case 2: 
        mostrar_inventario();
        break;
    case 3:
        Actualizar_activo();
        break;
    case 4:
        Monitoreo_de_ciclo_de_vida_del_activo();
        break;
    case 5:
        Historial_Movimiento();
    case 6: return;
    default:
        printf("\n[!]Opcion no valida. \n");
  }
}while(ops !=6); 

}

//-----------------------Funciones para el administrador-----------------------------//
void altaActivo(){
    
    ActivoFijo *nuevo = (ActivoFijo *)malloc(sizeof(ActivoFijo));

    //Validacion de apuntadores
    if (nuevo == NULL){
        printf("Error: No se pudo asignar memoria. \n");
        return;
    }
    printf("\n--- Registro de Activo ---\n");
    printf("ID unico: ");
    fgets(nuevo->id, 15, stdin); //fgets es como una lectura /scanf
    nuevo->id[strcspn(nuevo->id, "\n")] = 0; // Limpiar el salto de línea

    printf("Nombre del activo: ");
    fgets(nuevo->nombre, 50, stdin);
    nuevo->nombre[strcspn(nuevo->nombre, "\n")] = 0;

    printf("Costo inicial: ");
    scanf("%lf", &nuevo->costo_inicial);
    
    printf("Valor residual: ");
    scanf("%lf", &nuevo->valor_residual);
    
    printf("Vida util (anios): ");
    scanf("%d", &nuevo->vida_util);

    nuevo->estado = 1; // Por defecto: Operativo [cite: 104]
    nuevo->fecha_ingreso = time(NULL); // Captura fecha real del sistema [cite: 189]

    // Persistencia en Archivo Binario [cite: 221, 224]
    FILE *archivo = fopen("activos.dat", "ab");
    if (archivo != NULL) {
        fwrite(nuevo, sizeof(ActivoFijo), 1, archivo);
        fclose(archivo);
        printf("\nActivo guardado exitosamente en el archivo binario.\n");
    } else {
        printf("Error al abrir el archivo.\n");
    }

    free(nuevo); // Liberar memoria obligatoriamente [cite: 220]
}

void generarReporte() {
    ActivoFijo temp;
    FILE *archivo = fopen("activos.dat", "rb");
    
    if (archivo == NULL) {
        printf("No hay datos registrados.\n");
        return;
    }

    printf("\n%-15s %-20s %-15s %-15s\n", "ID", "Nombre", "Costo", "Valor Actual");
    printf("----------------------------------------------------------------------\n");

    while (fread(&temp, sizeof(ActivoFijo), 1, archivo)) {

        // Cálculo de depreciación lineal en tiempo real 
        double tiempo_transcurrido = difftime(time(NULL), temp.fecha_ingreso) / (365.25 * 24 * 3600);
        
        // Si el tiempo es menor a un año para pruebas, puedes dividir solo por 60 para ver cambios en segundos
        double depreciacion = ((temp.costo_inicial - temp.valor_residual) / temp.vida_util) * tiempo_transcurrido;
        double valor_actual = temp.costo_inicial - depreciacion;

        if (valor_actual < temp.valor_residual) valor_actual = temp.valor_residual; // Límite financiero [cite: 195]

        printf("%-15s %-20s %-15.2f %-15.2f\n", temp.id, temp.nombre, temp.costo_inicial, valor_actual);
    }
    
    fclose(archivo);
}

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//en esta funcion el administrador podra borrar activos que ya no tengan seguimiento en el  mercado
void eliminaractivo(){
	char id_buscar[15];
	ArchivoFijo temp;
	int encontrado = 0;

	FILE *archivo_open = fopen ("activos.dat", "rb+");
	if(archivo == NULL) {
		printf("\n[!] No existe el inventario registrado.\n");
		return;
	}

	printf("\n---------- Eliminar activo ----------\n");
	printf("Ingrese la ID del activo a eliminar: ");
	scanf("%s", id_buscar);

	while (fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1) {
		if (strcmp(temp.id, id_buscar) == 0) {
			encontrado = 1;
			
			if(temp.estado == 4){
				printf("\n[!] El activo '%s' ya se ha dado de baja.\n", temp.nombre);
				break;
			}

			printf("\n[*] Activo encontrado: %s\n", temp.nombre);

			//Eliminacion logica
			temp.estado = 4;

			fseek(archivo, -sizeof(ActivoFijo), SEEK_CUR); //Regresa a la posicion del activo
			fwrite(&temp, sizeof(ActivoFijo), 1, archivo); //Sobreescribe el registro

			printf("[*] Activo eliminado exitosamente del sistema. \n");
			break;
		}
}

if(encontrado == 0) {
printf("\n[!] Activo no encontrado.\n");
}

printf("----------------------------------------------------------------------");
fclose(archivo);
}


//-----------------------------------------------------------------------------------------------------------------------------------------///


//---------------------------------funciones para operador------------------------------------------------------//
void consultar_activo(){
    char id_buscar[15];
	int encontreado=0;
	
	// Variable temporal donde se cargará cada activo leído
	ActivoFijo temp;
	 // Abrimos el archivo en modo lectura binaria
	FILE *archivo=fopen("activos.dat", "rb");
	if(archivo==NULL){
	printf("\nNo existe inventario registrado.\n");
	return;
	}
    printf("\n========== CONSULTAR ACTIVO ==========\n");
	printf("Ingrese el ID del activo: ");
	scanf("%s", id_buscar);
	
	// Recorremos el archivo registro por registro
    while(fread(&temp, sizeof(ActivoFijo), 1, archivo)==1){   //fread()->Leer registros binarios.
		
        if(strcmp(temp.id, id_buscar)==0){
            encontreado=1;
			printf("\n========== ACTIVO ENCONTRADO ==========\n");
			printf("ID: %s\n", temp.id);
			printf("Nombre: %s\n", temp.nombre);
			printf("Descripcion: %s\n", temp.descripcion);
			printf("Costo inicial: %.2lf\n", temp.costo_inicial);
			printf("Valor residual: %.2lf\n", temp.valor_residual);
			printf("Vida util: %d anios\n", temp.vida_util);
            if(temp.estado==1){
                printf("Estado: Operativo\n");
            }
            else if(temp.estado==2){
                printf("Estado: Mantenimiento\n");
            }
            else if(temp.estado==3){
                printf("Estado: Depreciado\n");
            }
            else{
                printf("Estado: Desconocido\n");
            }
            printf("Fecha de ingreso: %s", ctime(&temp.fecha_ingreso)); // ctime()->convertir fecha a formato legible.
			printf("=======================================\n");
			break;
        }
    }
    if(encontreado==0){
	printf("\nNo se encontro el archivo."); 
		
	}
	fclose(archivo);
}

void mostrar_inventario(){
	ActivoFijo temp; //cajita temporal para guardar el nombre del archivo
	FILE *archivo = fopen("activos.dat", "rb");//Para abrir el archivo

	if(archivo == NULL){
		printf("No hay inventario registrado");
	}
	while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
		printf("ID: %s\n", temp.id);
		printf("Nombre: %s\n", temp.nombre);
		printf("Costo inicial: %.2lf\n", temp.costo_inicial);
		printf("Valor residual: %.2lf\n", temp.valor_residual);
		printf("Vida util: %d anios\n", temp.vida_util);

		if(temp.estado == 1){
			printf("Estado: Operativo");
	} else if (temp.estado == 2){
			printf("Estado: Mantenimiento");
		} else if (temp.estado == 3){
			printf("Estado: Depreciado\n");
				}
		printf("Fecha de ingreso: %s", ctime(&temp.fecha_ingreso));
		
			
}
		fclose(archivo); //cerrar el archivoahora explicame
}
void Actualizar_activo(){
	char id_buscar[15]; //Guarda el ID del usuario
	ActivoFijo temp; //fuarda temporalmente el activo
	int nuevo_estado; //Guarda el nuevo estado
	int encontrado = 0; //Dice si encontro el archivo

	FILE *archivo = fopen("activos.dat", "rb+");

	if(archivo == NULL){
		printf("No existe el inventario registrado");
		return;
	}
	printf("\n========= ACTUALIZAR ARCHIVO =========\n");

	printf("ingrese la ID del activo: ");
	scanf("%s", id_buscar);

	while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){
		if(strcmp(temp.id, id_buscar) == 0){
			encontrado = 1;

			printf("Activo encontrado: %s", temp.nombre);

			printf("Seleccione el uevo estado:\n");
			printf("1. Operativo");
			printf("2. Mantenimiento");
			printf("3. Depreciado");

			printf("Opcion: ");
			scanf("%d", &nuevo_estado);

			temp.estado = nuevo_estado;

			fseek(archivo, -sizeof(ActivoFijo), SEEK_CUR); //Regresar a la posicion del archivo

			fwrite(&temp, sizeof(ActivoFijo), 1, archivo); //Sobreescribir el registro

			printf("Activo actualizado correctamente");
			break;
	}
	}
if(encontrado == 0){
	printf("Activo no encontrado");
}
	fclose(archivo);
}


void Monitoreo_de_ciclo_de_vida_del_activo(){
    ActivoFijo temp; // guardamos archivo temporalmente
    double tiempo_transcurrido;
    double depreciacion;
    double valor_actual;

    FILE *archivo = fopen("activos.dat", "rb");

    if(archivo == NULL){
        printf("No existe inventario registrado.");
        return;
    }

    printf("\n====== MONITOREO DE CICLO DE VIDA ======\n");

    while(fread(&temp, sizeof(ActivoFijo), 1, archivo) == 1){ //Leer archivos uno por uno

        // Tiempo transcurrido en años
        tiempo_transcurrido =
        difftime(time(NULL), temp.fecha_ingreso)
        /
        (365.25 * 24 * 3600);

        // Calcular depreciación
        depreciacion =
        ((temp.costo_inicial - temp.valor_residual)
        / temp.vida_util)
        * tiempo_transcurrido;

        // Valor actual
        valor_actual =
        temp.costo_inicial - depreciacion;

        
        if(valor_actual < temp.valor_residual){ // Evitar valores negativos
            valor_actual = temp.valor_residual;
        }

        printf("Activo: %s\n", temp.nombre);
        printf("ID: %s\n", temp.id);

        printf("Tiempo en uso: %.2lf anios\n", tiempo_transcurrido);

        printf("Valor actual: %.2lf\n", valor_actual);

        // Estado del ciclo de vida
        if(tiempo_transcurrido >= temp.vida_util){
            printf("Estado del ciclo: Depreciado\n");

        }
        else if(tiempo_transcurrido >=
        temp.vida_util * 0.75){
            printf("Estado del ciclo: Cerca del fin de vida\n");
        }
        else{
            printf("Estado del ciclo: Activo en funcionamiento\n");
        }  
    }
    fclose(archivo);
}

void Historial_Movimiento(){
	/* OBJETIVO:
     Mostrar un historial general de los activos almacenados
     indicando su estado actual y fecha de ingreso.*/

    ActivoFijo temp;
	  // Contador de movimientos
	int movimientos=0;
	FILE *archivo=fopen("activos.dat", "rb");
    if(archivo==NULL){
	printf("\nNo existe historial disponible.\n");
	return;
	}
    printf("\n========== HISTORIAL DE MOVIMIENTOS ==========\n");
	 // Leer cada registro del archivo
    while(fread(&temp, sizeof(ActivoFijo), 1, archivo)==1){
        printf("Activo: %s\n", temp.nombre);
		printf("ID: %s\n", temp.id);
		printf("Fecha de ingreso: %s", ctime(&temp.fecha_ingreso));
        if(temp.estado==1){
            printf("Movimiento: Activo actualmente operativo.\n");
        }
        else if(temp.estado==2){
            printf("Movimiento: Activo enviado a mantenimiento.\n");
        }
        else if(temp.estado==3){
            printf("Movimiento: Activo depreciado.\n");
        }
        else{
            printf("Movimiento: Estado desconocido.\n");
        }
        movimientos=movimientos+1;
    }
    printf("Total de movimientos registrados: %d\n", movimientos);
	fclose(archivo);
}


//en esta funcion tambien generara un .txt con los activos registrados 
void reporteentexto();





