/**
 * Autor - Rodrigo Quiroz Reyes
 * Fecha - 10/09/2020
 * Actividad individual 4: Procesos 
 * Ejercicio - Creación de hijos y calculo de promedio de id´s
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
  int id_hijo;
  int promedio;
  int pipe[2];
}Proceso;//Estructura libro para simbolizar el tipo de dato struct

int procesos_fork(Proceso*,int);//Método para realizar todo el método de la creación de forks
void imprimir_histograma(Proceso*,int,int); //Método para imprimir el histograma
int proporcion(int);//El cálculo que hace una proporción a los datos a la hora de imprimir el histograma
int leer(int*);//Método para leer desde un pipe
void escribir(int*, int );//Método para escribir desde un pipe

int main(int argc, char * const * argv) {

    char* cvalue; // Extracción de el valor del inpu
    int c; //Varible para obtener el argumento de la command line
    Proceso *procesos;
  
    while ((c = getopt (argc, argv, "n:")) != -1){ //Obtención del arg de command line
    cvalue =optarg;
    }

    int no_hijos=atoi(cvalue); //Conversión del valor a int

    //Malloc de la estructura
    procesos=(Proceso*)malloc(no_hijos*sizeof(Proceso));

    int top_prom=procesos_fork(procesos,no_hijos); 

    imprimir_histograma(procesos,top_prom,no_hijos);

    free(procesos);
    
    return 0;
    
}

/**
 * Metodo para la creación de procesos hijos y el cálculo del promedio entr procesos. 
 * A su vez asigna los datos de promedio y pid para después ser utilizados en el histograma
 * 
 * @param Procesos* Estructura que almacena el array de promedios y id de hijos
 * @param int no_hijos El tamaño de los arreglos
 * @return int Regresa el promedio más alto
 */
int procesos_fork(Proceso* procesos, int no_hijos)
{    
    pid_t pid; //Proceso inicial
    Proceso *aux=procesos;//Apuntador auxiliar que nos ayuda en el loop
    int i=0;//Counter para dar formato a algunos printf
    int promedio_top=0;//Variable para almacenar el promedio más alto

    for(;aux<(procesos+no_hijos);++aux,i++)
    {
        pipe(aux->pipe);//Creación del pipe
        int promedio;//Variable para manejar el cálculo de los promedios
        pid=fork();//Creación del proceso hijo
        aux->id_hijo=pid;//Asignación de pid

        if(pid==-1)//Condición en caso de error
        {
            printf("Hubo un error a la hora de crear al hijo\n");
            printf("Se han creado %d hijos\n", i+1);
            aux=(procesos+no_hijos);//Se aumenta hasta el final del ciclo
        }
        else if(pid==0)//Condición de las acciones del hijo
        {
            sleep(1);
            promedio=(getpid()+getppid())/2;
            printf("Soy el proceso hijo con PID = %d, mi PPID es %d y mi promedio es = %d\n",getpid(),getppid(),promedio);
            escribir(aux->pipe,promedio); //Escribe el promedio a través de un pipe
            exit(promedio);//Método para terminar el hijo
        }
    }

    for (aux=procesos;aux<(procesos+no_hijos);++aux,i++)
    {
        int status;
        
        if(waitpid(aux->id_hijo,&status,0)!=-1)//El proceso padre espera a que el hijo acabe
        {      
            aux->promedio=leer(aux->pipe);//Lectura del promedio a través del pipe      
            if(aux->promedio>promedio_top) promedio_top=aux->promedio; //Asigna el promedio más alto
        }

    }
    return promedio_top;
}

/**
 * Metodo para la modificar el size con base a un límite de impresiones de asterisco
 * 
 * @param int top El promedio más alto
 * @return int La proporcióna usar en la impresión
 */
int proporcion(int top)
{
    for (int i=5;i<top;i+=5)
    {
        if((top/i)<50) //Máximo de 50 asteriscos
        {
            return i;
        }
    }
}

/**
 * Metodo para imprimir el histograma mediante los arreglos dados
 * 
 * @param Proceso* Estructura que almacena el array de promedios y id de hijos
 * @param int no_hijos El tamaño de los arreglos
 */
void imprimir_histograma(Proceso* proceso,int top_prom,int no_hijos)
{
    Proceso*aux=proceso;//Apuntador auxiliar que nos ayuda en el loop
    int i=proporcion(top_prom);

    printf("\n%s\t%s\t%s\n","PID Hijo", "Promedio","Histograma\n");

    for (; aux < (proceso+no_hijos); aux++)
    {
        printf("  %d\t\t  %d\t\t",aux->id_hijo,aux->promedio);
        for(int j=0;j<(int)(aux->promedio/i);j++) printf("*"); //Impresión de los asteriscos
        printf("\n");
    }
}

/**
 * Metodo para leer el pipe entre los procesos y regresar el promedio
 * 
 * @param int* fd Entrada del archivo por donde el pipe va a leer
 * @return int El promedio que extrajo
 */
int leer(int *fd)
{
    int i;
    close(fd[1]);//Se cierra el otro extremo del pipe
    read(fd[0], &i, sizeof(int)); //Lectura del pipe
    return i;
}

/**
 * Metodo para escribir el promedio mediante el pipe entre los procesos
 * 
 * @param int* fd Entrada del archivo por donde el pipe va a escribir
 * @param int prom
 */
void escribir(int *fd, int prom)
{
    close(fd[0]);//Se cierra el otro extremo del pipe
    write(fd[1], &prom, sizeof(int)); //Escritura del pipe
    return;
}
