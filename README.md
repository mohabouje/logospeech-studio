# LogoSpeech Studio. #
## Una manera sencilla de mejorar la investigación lingüística.##

LogoSpeech Studio es una herramienta para el procesado de señales bioacústicas en tiempo real. Esta diseñado para el apoyo en la docencia logópeda, centrándose en 
soportes de ayuda para el reconocimiento y vocalización del habla.

**Versión 0.0**

### ¿ Qué diferencia a LogoSpeech? ###

LogoSpeech Studio es un proyecto de software libre multiplataforma desarrollado en C/C++ con las librerias gráficas de Qt. Nace con la finalidad de servir como heraamienta de entrenamiento para el reconocimiento del habla y correcta vocalización para personas con problemas auditivos.

Se basa en el análisis temporal/espectral de la señal del habla para la extración de parámetros, entre los más destacados:




* Enegía de la señal.
* Tasa de cruces por cero.
* Detección de la frecuencia fundamental del hablante.
* Detección de sonoridad de las tramas de la señal.
* Espectro real FFT de la señal.
* Espectro LPC de la señal.
* Cepstrum de la señal.
* Espectro mediante banco de filtros a diferentes escalas.



Ha sido testeada y compilada en diferentes sistemas operativos:

1. Windows (Windows 7 o superior).
2. Mac Os (Mac Os Lion o superior).
3. Linux (Ubuntu 12 o superior).

## ¿Como compilar la aplicación? ##

La aplicación ha sido optimizada para que sea totalmente compatible con cualquier sistema operativo que soporte las librerias nativas de Qt. Simplemente, descarga el repositorio añade las dependencias y proceda a la compilación. 

**Dependencias:** requerida la libreria FFTW. [http://www.fftw.org/](http://www.fftw.org/)


### Contribuciones ###

* Intructor: [Ángel de la Torre Vega](http://dtstc.ugr.es/~atv/Paginas/personal.html)
* Programador: Mohammed Boujemaoui Boulaghmoudi 

### Contacto ###

* Mohammed Boujemaoui, [mohaboujemaoui@gmail.com](mailto:mohaboujemaoui@gmail.com)

# LICENCIA #

```
#!c++

    LogoSpeech Studio 
    Copyright © 2015 Mohammed Boujemaoui

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
```