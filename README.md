# ZARPA creador de aventuras

- [Presentación de Zarpa](#presentación-de-zarpa)

## Presentación de Zarpa

Zarpa es un proyecto que estoy desarrollando para crear aventuras, no tiene lo que se dice un IDE. Todo funciona a través de sencillos ficheros de texto, el típico formato INI, por lo que con tener un editor que permita tener múltiples pestañas y mejor aún si crea una vista a cada lado como Notepad++ es suficiente para trabajar.

**Zarpa es muy complejo (que no complicado), por lo que recomiendo que si te interesa el proyecto leas toda la documentación que hay en el directorio doc/.**

Estas son algunas de sus características, algunas sin implementar todavía y otras no totalmente funcionales.

- Lenguaje script propio basado en el de PAWS llamado Zarpascript, pero sin sus limitaciones: No más "números mágicos" para nombrar localidades y objetos, no límites para crearlos, variables enteras de 32bits. Nuevo concepto de entidad, para manejar NPC (o PSI como se conocen en PAW). Lenguaje sencillo, sin bucles ni sentencias complicadas. Se han rediseñado las instrucciones de PAW para simplificarlas y adaptarlas a los nuevos tiempos.

- Depurador para seguir la ejecución del Parser con posibilidad de crear breakpoints (en desarrollo).

- Pensada para ser multiidioma. Los textos van separados del código por lo que puede traducirse cualquier aventura sin tener que tocar nada del código. Esto permite que cualquiera sin conocimientos de programación pueda traducir, además al estar los textos aparte, no hay peligro de modificar accidentamente código sensible.

- Parser compatible con idioma Inglés (en desarrollo) e idioma Español. Es muy sencillo añadir nuevos idiomas sin tocar el código del Parser.

- Todo se crea a través de sencillos ficheros de texto plano, por lo que no es dependiente de ningún programa. Basta un simple editor de textos como Notepad++, FeatherPad, Geany...

- Multiplataforma y ligero. Compatible con Windows y Linux. Zarpa es ligero en recursos, está escrito íntegramente en C++ estándar y la única dependencia que tiene son las librerías gráficas SFML para el GUI.

- Interface gráfica aparte muy personalizable: fuentes TTF, colores, posición de los distintos elementos... casi todo puede modificarse en ficheros de temas que no son más que simples INIs.

- Posibilidad de usar modo consola (en desarrollo).

- Estructura cliente/servidor separando la parte gráfica GUI del parser, para acceder remotamente (en desarrollo).

Estos son los tipos de datos que maneja Zarpa. Cada aventura que diseñes tendrá con seguridad todos ellos. La nomenglatura, o el cómo se nombran **es sólo una recomendación**, yo indico la que usa Zarpa por defecto para tener un código más coherente, eres libre de nombrar las cosas como quieras por supuesto.

- **Objetos.** Poco puedo contar de ellos, imprescindibles para cualquier aventurero. Se definen en **objects.ini**. Se nombran con prefijo obj, ej. "objAntorcha".

- **Localidades.** Son las "habitaciones" por donde irá el jugador, se definen en **locations.ini**. _locPlayer apuntará siempre a la localidad actual del jugador. Son más dinámicas que en PAW ya que tienen la posibilidad de cambiar la descripción e imagen durante la ejecución. Se nombran con prefijo loc, ej. "locEntradaCueva".

- **Entidades.** Sería lo que PAW definió como PSI (Personajes Seudo Inteligentes), lo que ahora se llama NPC. Pero en Zarpa se llaman entidades. Se definen en **entities.ini**. Aún es una fase Beta, pero pueden hacer cosas muy potentes con pocas lineas de código. Se nombran con prefijo ent, ej. "entArchivero".

- **Jugador.** Podríamos decir que el jugador es una entidad con algunas características únicas. Puedes acceder a ella a través de _entPlayer.

- **Variables.** Permiten almacenar y manipular números enteros de 32 bits, o lo que es lo mismo valores entre -2.147.483.648 al 2.147.483.647. Se definen en **variables.ini**.

Ahora los siguientes componentes de la aventura y seguramente los más importantes para crear una buena historia. Ten en cuenta que están separados del resto de datos en el directorio es/ (en/ para inglés). Esto es así porque Zarpa se ha pensado para ser lo más versátil a la hora de traducir aventuras. Se verá cómo traducir una aventura en **conceptos avanzados**.

- **Vocabulario.** Todo el vocabulario está definido por IDs en **es/vocabulary.ini**. Estos ID, van en MAYÚSCULAS. Son similares a las entradas de vocabulario que usaba PAW.

- **Mensajes.** Se crean en **es/messages.ini**. En Zarpa un mensaje es más que un simple texto. Lo veremos en este tutorial. Se nombran con el prefijo msg, ej. "msgNoPuedoHacerlo". Los mensajes predefinidos del sistema empiezan por _msg, por ejemplo el clásico "No puedo hacer eso..." está en _msgCantDoIt. Todos los mensajes del sistema son totalmente redefinibles.

- **Descripciones.** Se crean en **es/descriptions.ini**. Si no lo sabes ya, es lo que se muestra cuando el jugador entra en una localidad. Se nombran con el prefijo desc. "desEntradaCueva"