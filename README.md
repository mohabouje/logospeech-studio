# LogoSpeech Studio. #
## Una manera sencilla de mejorar la investigación lingüística.##

LogoSpeech Studio es una herramienta para el procesado de señales bioacústicas en tiempo real. Esta diseñado para el apoyo en la docencia logópeda, centrándose en 
soportes de ayuda para el reconocimiento y vocalización del habla.

**Versión 0.0**

### ¿ Qué diferencia a LogoSpeech? ###

LogoSpeech Studio es un proyecto de software libre multiplataforma desarrollado en C/C++ con las librerias gráficas de Qt. Nace con la finalidad de servir como heraamienta de entrenamiento para el reconocimiento del habla y correcta vocalización para personas con problemas auditivos.

Se basa en el análisis temporal/espectral de la señal del habla para la extración de parámetros, entre los más destacados:


```
#!c++

* Enegía de la señal.
* Tasa de cruces por cero.
* Detección de la frecuencia fundamental del hablante.
* Detección de sonoridad de las tramas de la señal.
* Espectro real FFT de la señal.
* Espectro LPC de la señal.
* Cepstrum de la señal.
* Espectro mediante banco de filtros a diferentes escalas.
```


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

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

```
#!c++

LogoSpeech Studio  uses code from Aubio Library sample available 
 on Aubio Library page:
http://aubio.org/
```