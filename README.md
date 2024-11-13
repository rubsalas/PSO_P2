# PSO_P2
Proyecto 2 del curso CE-4303 Principios de Sistemas Operativos del programa de Licenciatura en Ingeniería en Computadores del Instituto Tecnológico de Costa Rica.

## Proyecto Específico por Definir


### Raspberry Pi

1. Para instalar un sistema operativo en el Raspberry Pi se necesitará el software de Raspberry Pi Imager.

Descargar de [Raspberry Pi](http://www.raspberrypi.com/software/)

O bien, se puede instalar por medio de una terminal
```bash
sudo apt install rpi-imager
```


2. Crear la imagen del sistema operativo

Se estará instalando el sistema operativo de Ubuntu en la tarjeta.

Se debe buscar la aplicación del Imager y abrirla. Ahí se escogerá el sistema operativo Ubuntu Desktop (64-BIT). Se necesitará una tarjeta micro sd para flashear la imagen, esta debe ser escogida en el programa.

Con esto se puede comenzar a escribir en la tarjeta.

Al finalizar de crear la imagen en la tarjeta se ejecta del computador y se ingresa a la Raspberry Pi.

Para este proyecto se estará utilizando una Raspberry Pi 5 con 8 Gb de RAM.


3. Bootear el sistema operativo

Con la tarjeta ya ingresada en su slot correspondiente en la Raspberry Pi, se procede a conectarla a la corriente. Esto hará que haga el boot desde la tarjeta micro sd.

Seguir las indicaciones para configurar el sistema operativo. Para el proyecto se estará utilizando Ubuntu. 


4. Actualizar el sistema operativo

Como recomendación actualizar el sistema operativo recién instalado si hay oportunidad,


5. Utilizando el Raspberry Pi

Si se conecta por HDMI a un monitor, este funcionará como cualquier otro computador manejado por un teclado y un mouse. Aún así, se le configurará una conexión para accederlo por ssh.


6. Configuración de red estática

Ir a la parte de la red cableada en la configuración de red.

En la parte de IPv4:

Asignar el método manual

**Asignar una Dirección:**

Dirección: 192.168.50.180

Máscara de red: 255.255.255.0

Puerta de enlace: 192.168.50.1

**Configurar el DNS:**

Desmarcar el automático

Asignar el 192.168.50.1

Desconectarse y volverse a conectar para que los cambios ocurran


7. Descarga de dependencias para la configuración del acceso

Instalar dependencias de ssh
```bash
sudo apt-get install ssh
```


8. Configuración del computador que se conectará al Raspberry Pi

Para que un computador pueda ser capaz de ingresar y manejar el Raspberry Pi se necesita que este pueda tener acceso al ssh.

Se crea una llave para el ssh
```bash
ssh-keygen
```
no asignar file
dejar empty el passphrase

Se crea el key de forma aleatoria

Se procede a copiar la llave que se acaba de crear en el computador, por medio del protocolo SCP
```bash
scp .ssh/id_rsa.pub rpiUser@192.168.50.180:/home/rpiUser
```

Ahora, en el Raspberry.

Crear directorio ".ssh" para que el computador pueda obtener el key al conectarse
```bash
mkdir .ssh
```

Se le otorgan los permisos al folder
```bash
chmod 700 .ssh
```

Se mueve el archivo dentro de .ssh como "authorized_keys"
```bash
mv id_rsa.pub .ssh/authorized_keys
```

Para acceder al Raspberry Pi desde el computador

Se hace una conexión vía ssh
```bash
ssh rpiUser@192.168.50.180
```

Ahora se puede controlar la Raspberry Pi a la cual se conectó.

Para terminar la conexión.
```bash
exit
```

Para facilitar la conexión, se puede editar el archivo hosts.
```bash
sudo nano /etc/hosts
```

Se abrirá el archivo para editar y se debe agregar la dirección ip del Raspberry Pi.
```txt
192.168.50.180 rpi5 rpi5
```

Ahora se podrá conectar por ssh a partir del nombre guardado, pero con el usuario del nodo.
```bash
ssh rpiUser@rpi5
```
Si tienen el mismo usuario, no es necesario ponerlo al conectarse.
```bash
ssh rpi5
```
Para terminar la conexión, se mantiene el mismo comando.
```bash
exit
```


### Instalación y Configuración OpenMPI y Cluster


1. Instalación de paquetes básicos en TODOS los nodos

Descargar la versión más estable de [Open MPI](https://www.open-mpi.org/software/)

Crear un directorio en $HOME donde instalarlo
```bash
mkdir $HOME/openmpi
```

Mover el archivo comprimido descargado al directorio recién hecho
```bash
mv $HOME/Downloads/openmpi-X.X.X.tar.gz $HOME/openmpi/
```

Ir al directorio creado por medio de la terminal
```bash
cd $HOME/openmpi/
```

Extraer el paquete
```bash
tar -xzvf openmpi-X.X.X.tar.gz
```

Ir al directorio donde se descomprime el paquete
```bash
cd $HOME/openmpi/openmpi-X.X.X
```

Instalar build-essential
```bash
sudo apt-get install build-essential
```

Configurar donde instalar Open MPI. "--enable-heterogeneous" se utiliza para poder compilar en arm y que pueda correr con x86.
```bash
sudo ./configure --enable-heterogeneous --with-internal-pmix --prefix=/usr
```

Compilar el paquete de MPI en paralelo, utilizando todos los nucleos de la CPU para completar la compilación más rápido.
```bash
sudo make -j$(nproc) all
```

Una vez compilado, se instala
```bash
sudo make install
```

Instalar paquetes complementarios para MPI
```bash
sudo apt-get install openmpi-bin libopenmpi-dev
```

Agregar Open MPI a las variables de entorno
```bash
echo 'export PATH=/usr/bin:$PATH' >> ~/.bashrc
```

```bash
echo 'export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
```

Recarga el archivo ~/.bashrc para actualizar las variables de entorno
```bash
source ~/.bashrc
```

Verificar la instalación con la versión del compilador de c en open mpi
```bash
mpicc -v
```

Como se tendrá que utilizar mpicc con sudo, se debe agregar a su PATH

Para que sudo incluya el PATH actualizado
```bash
sudo env "PATH=$PATH" mpicc --version
```

Edita el Archivo de Configuración de sudoers
```bash
sudo visudo
```

Agrega el PATH de OpenMPI. Buscar la línea que comienza con Defaults secure_path y agregar el directorio de OpenMPI (/home/_user_/openmpi/bin) al final de esta línea
```bash
Defaults secure_path="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
```

Verificar que tenga Heterogeneous support
```bash
ompi_info | grep Heterogeneous
```

Si hay problemas con la instalación y configuración

Eliminar archivos de compilación previos desde el directorio de OpenMPI
```bash
make clean
```

Ejecuta un distclean, que es una limpieza más profunda:
```bash
make distclean
```

Para desinstalarlo
```bash
make uninstall
```

Asegurarse donde se instaló OpenMPI
```bash
which mpirun
```

Elimina todos los archivos de OpenMPI. Si está en /usr/bin/, ejecutar:
```bash
sudo rm -rf /usr/bin/mpi*
sudo rm -rf /usr/include/mpi*
sudo rm -rf /usr/lib/libmpi*
sudo rm -rf /usr/share/openmpi
sudo rm -rf /usr/etc/openmpi
```



2. Configuración de red estática en el nodo esclavo

Ir a la parte de la red cableada en la configuración de red (nodo esclavo)

En la parte de IPv4:

Asignar el método manual

**Asignar una Dirección:**

Dirección: 192.168.122.101

Máscara de red: 255.255.255.0

Puerta de enlace: 192.168.122.1

**Configurar el DNS:**

Desmarcar el automático

Asignar el 192.168.50.1

Desconectarse y volverse a conectar para que los cambios ocurran


3. Descarga de dependencias para la configuración del cluster

Instalar dependencias en el nodo esclavo
```bash
sudo apt-get install ssh
```
```bash
sudo apt-get install nfs-common portmap
```

Instalar dependencias en el nodo maestro
```bash
sudo apt-get install nfs-kernel-server nfs-common portmap
```


4. Configuración de red estática en el nodo maestro

Si ya se hizo en la configuración del Raspberry, saltar esta parte.

Ir a la parte de la red cableada en la configuración de red

En la parte de IPv4:

Asignar el método manual

**Asignar una Dirección:**

Dirección: 192.168.50.180

Máscara de red: 255.255.255.0

Puerta de enlace: 192.168.50.1

**Configurar el DNS:**

Desmarcar el automático

Asignar el 192.168.50.1

Proceder a conectar el nodo maestro en la red cableada para que el LAN funcione


5. Configuración del lado del nodo maestro

Para que el maestro sea capaz de ejecutar comandos en cada uno de los nodos esclavos se necesita que estos últimos puedan tener acceso al ssh.

Se crea una llave para el ssh
```bash
ssh-keygen
```
no asignar file
dejar empty el passphrase

Se crea el key de forma aleatoria

Revisar el nombre del public key que se acaba de crear, en este caso fue id_ed25519.pub. Este está guardado en la carpeta .ssh/.

Se procede a copiar la llave que se acaba de crear al nodo esclavo, por medio del protocolo SCP
```bash
scp .ssh/id_ed25519.pub slaveUser@192.168.50.102:/home/slaveUser
```

Ingresar la clave del computador al que se está enviando, si es necesario.

Ahora en el nodo esclavo

Crear directorio .ssh para que el nodo maestro pueda obtener el key al conectarse
```bash
mkdir .ssh
```

Se le otorgan los permisos al folder
```bash
chmod 700 .ssh
```

Se mueve el archivo dentro de .ssh como "authorized_keys"
```bash
mv id_ed25519.pub .ssh/authorized_keys
```

Si al correr el programa paralelizado da problemas la conexión, intentar hacer este paso anterior solo con este comando, el usuario del esclavo y el ip del esclavo
```bash
ssh-copy-id slaveUser@192.168.122.101
```

Para acceder al nodo esclavo desde el nodo maestro

Se hace una conexión vía ssh
```bash
ssh slaveUser@192.168.122.101
```

Ahora se puede controlar la máquina a la cual se conectó.

Para terminar la conexión
```bash
exit
```

Se deben configurar así cada uno de los nodos.

Editar los archivos hosts
```bash
sudo nano /etc/hosts
```

Se abrirá el archivo para editar y se deben agregar las direcciones ip del master y los nodos
```txt
192.168.50.100 master master
192.168.122.101 node1 node1
192.168.50.102 node2 node2
192.168.122.103 node3 node3
```

Ahora se podrá conectar por ssh a partir del nombre guardado, pero con el usuario del nodo
```bash
ssh slaveUser@node1
```
Si tienen el mismo usuario, no es necesario ponerlo
```bash
ssh node2
```


6. Configuración del servidor de archivos NFS

En cada nodo del cluster, se necesita que todos los programas distribuidos por ejecutar tengan a disposición una carpeta compartida entre ellos. Esto será posible con la librería instalada anteriormente, nfs-kernel-server.

Se configurará el recurso compartido

Se creará un directorio en el home del nodo maestro. Este debería quedar a la par del directorio con nombre del usuario en /home/clusterdir/
```bash
cd ..
sudo mkdir clusterdir
```

Como root se exportará el directorio para que se pueda acceder remotamente, a través del archivo export
```bash
sudo nano /etc/exports
```

Se abrirá para editar el archivo. Se debe agregar la ruta del directorio de la carpeta en el nodo maestro y las ips de los nodos esclavos
```txt
/home/clusterdir 172.18.180.182(rw,no_subtree_check,async,fsid=0,no_root_squash,insecure)
192.168.50.102
```

Se debe reiniciar el servicio de nfs
```bash
sudo /etc/init.d/nfs-kernel-server restart
```

Para verificar que se pueda acceder a la carpeta compartida desde el nodo esclavo se utiliza el comando showmount con el ip del nodo master
```bash
showmount -e 172.18.138.190
192.168.50.180
```

Se montará el recurso desde el cli para ser agregado a fstab. Así se hará el montado automático para que cada vez que arranque el nodo se cree en /home del nodo el directorio clusterdir igual que el nodo maestro. Ahí estarán los recursos compartidos del nodo maestro a través de nfs

Se creará el directorio en el nodo esclavo
```bash
sudo mkdir ../clusterdir
```

Se abre el archivo fstab para editarlo
```bash
sudo nano /etc/fstab
```

Se modifica el archivo fstab agregando al final el ip del nodo maestro, la dirección del directorio del nodo maestro, la dirección del directorio del nodo esclavo
```txt
172.18.138.190:/home/clusterdir /home/clusterdir nfs rw,sync,hard,intr 0 0
192.168.50.180
```

Se monta
```bash
sudo mount -a
```

Para verificar la conexión
```bash
mount
```

Si se desea desmontar (no hacer)
```bash
sudo umount -a
```


7. Configuración del entorno de desarrollo

El paquete de build-essentials tiene lo necesario para desarrollar y compilar, que ya se intenta instalar al inicio (usualmente ya viene en el SO)


8. Configuración para MPI

Continuando dentro del directorio /home/clusterdir/

Se crea un archivo de compilación .mpi_hostfile en el nodo maestro

```bash
sudo nano .mpi_hostfile
```

Se edita y se le agrega
```txt
#Nodo maestro
localhost slots=1

#Nodo esclavo1
node1 slots=1

#Nodo esclavo2
node2 slots=1

#Nodo esclavo3
node3 slots=1
```

Cada slot es la cantidad de procesos que cada nodo puede ejecutar. Esto depende de la cantidad de cores del computador.

Aquí se puede realizar ya una prueba


9. Compilación del código por paralelizar

El código por correr paralelamente debe estar dentro de la carpeta compartida junto a los archivos que necesite.

Debe incluir la libreria mpi y sus comandos para correrlo paralelamente

Se debe compilar con el compilador de mpi
Para este proyecto se codificó en el lenguaje c por lo que se usrá así
```bash
mpicc -o <executable_name> <program_code_file> -lm
```

Por ejemplo:
```bash
sudo mpicc -o gaussian_blur_mpi gaussian_blur_mpi.c -lm
sudo env "PATH=$PATH" "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" mpicc -o hello_mpi_x86 mpi_test/hello_mpi.c
```

Se utiliza sudo por estar en un directorio con permisos privilegiados


10. Ejecución del programa paralelizable con mpi

Su binario ejecutable debe estar dentro de la carpeta compartida por nfs

Para correrlo con mpi:
```bash
mpirun -np <number_of_processes> --hostfile <hostfile_name> ./<executable_name> <upper_limit>
```

Por ejemplo:
```bash
mpirun -np 4 --hostfile .mpi_hostfile ./gaussian_blur_mpi
mpirun -np 1 --hostfile ../.mpi_hostfile_aarch64 hello_mpi_aarch64 : -np 1 --hostfile ../.mpi_hostfile_x86 hello_mpi_x86
```


11. Para volver a correr el programa después de apagar las computadoras o si se pierde la conexión del mount nfs

Reiniciar el servicio de nfs en el nodo master
```bash
sudo /etc/init.d/nfs-kernel-server restart
```

Verificar que el nodo master siga compartiendo la carpeta por nfs en el nodo esclavo
```bash
showmount -e 192.168.50.100
```

Volver a hacer el mount en el nodo esclavo
```bash
sudo mount -a
```

Se recomienda salirse de la carpeta /home/clusterdir/ y volver a ingresar, si se encuentra en esta, para que los archivos carguen nuevamente

