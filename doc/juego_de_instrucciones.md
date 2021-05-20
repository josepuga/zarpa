# Zarpascript: Juego de instrucciones (en progreso)

- [Leyenda](#leyenda)
- [Manejo de variables](#manejo-de-variables)
- [Manejo de objetos](#manejo-de-objetos)

## Leyenda

Cada instrucción la indico en un formato como este ejemplo de **atloc**:

```
[!]atloc <loc> <ent>|<obj>|<word_obj_noun>

(*) Fijate en un espacio entre <loc> y <ent>, ya que esta instrucción admite 2 parámetros

[!] Lo que hay entre corchetes es opcional, eso quiere decir que es una intrucción condicional. Todos los condicionales tienen esta característica, y el ! implica negación, se puede poner:
     atloc (en la localidad) o 
     !atloc no en la localidad. 

<tipo> Indica el tipo de dato que admite la instrucción. Puede ser
    <obj> ID Objeto
    <ent> ID Entidad
    <des> ID Descripción
    <var> Variable
    <num> Número entero de 32 bits.
    <msg> ID Mensaje
    <loc> ID Localidad
    <zon> ID Zona
    <img> ID Imagen
    <snd> ID Sonido
    <word_obj_noun> ID de vocabulario del nombre de un objeto. (*)
    <word_verb_mov> ID de vocabulario de un nombre de movimiento. (*)
    <...> Algunas más referidas a vocabulario. (en preparación documentarlas).


| Este símbolo indica que el parámetro es exclusivo, o uno u otro. En el ejemplo debe ser una entidad, un objeto o una palabra de nombre de objeto.

(*) Hay "palabras comodín" que pueden usarse en estos parámetros:
   _VERB : Lleva siempre el verbo actual.
   _MOV : Lleva siempre el verbo de movimiento de la frase actual. (si lo hubiera)
   _OBJ : Lleva siempre el ID de la entrada de vocabulario del objeto actual (si lo hubiera).
   Hay algunos más (en preparacion documentarlos).
```

## Manejo de variables
```
[!]eq <var> <var>|<num>

EQual (igual). Compara si la variable es igual a variable o número.
```

```
[!]le <var> <var>|<num>

Less or Equal (menor o igual).
```

```
[!]ge <var> <var>|<num>

Greater or Equal (mayor o igual).
```

```
set <var> <var>|<num>

Iguala la variable con variable o número.
```

```
add <var> <var>|<num>

Añade a variable el valor de variable o número.
```

```
sub <var> <var>|<num>

Sustrae a variable el valor de variable o número.
```

## Manejo de objetos

```
[!]here <obj>|<word_obj_noun>

Si el objeto referido está en la localidad actual, llevado o puesto.
```

```
[!]empty <obj>|<loc>|<ent>|<word_obj_noun>

Si el objeto, localidad o entidad están vacíos, es decir no tienen ningún objeto.
```

```
put <obj>|<word_obj_noun> <obj> 

Pone el objeto en objeto. 

Si el objeto destino no es un contenedor se muestra el mensaje del sistema _msgNotContainer y hace notdone.
```

```
drop <obj>|<word_obj_noun> <loc>

Deja "caer" el objeto en la localidad. Al contrario que en PAW, aquí hay que especificar localidad destino, que generalmente será _locPlayer. 

Si el objeto no es llevado se muestra el mensaje del sistema _msgNotCarried y hace notdone.
```

```
get <obj>|<word_obj_noun> <loc>

Coge el objeto (lo deja en _locCarried) DESDE la localidad indicada. Al contrario que en PAW, aquí hay que especificar la localidad origen, que generalmente será _locPlayer.

Si el objeto ya está llevado o puesto, se muestra en mensaje _msgAlreadyCarried y hace notdone.

Si el objeto no está en la localidad indicada, se muestra el mensaje _msgGetNothing y hace un notdone.

Si el objeto está pero no es llevable, se muestra el mensaje _msgNotCarriable y hace notdone.
```

```
remove <obj>|<word_obj_noun>

Quita el objeto de llevado (_locWorn) y lo pone en puesto (_locCarried).

Si el objeto no se lleva puesto muestra el mensaje _msgNotWorn y hace notdone.
```

```
wear <obj>|<word_obj_noun>

Se pone el objeto llevado (pasa de _locCarried a _locWorn).

Si el objeto ya se lleva puesto muestra el mensaje _msgAlreadyWorn y hace notdone.

Si el objeto no se lleva muestra el mensaje _msgNotCarried y hace notdone.

Si el objeto no es ponible muestra el mensaje _msgCantWear y hace notdone.
```

```
swappos <obj> <obj>

Intercambia la posición (localidad) de ambos objetos.
```

```
exam <obj>|<word_obj_noun>

Examina el objeto, (muestra el mensaje que tiene asignado).

Si no tiene ningún mensaje asignado muestra el mensaje _msgNothingInteresting.

Esta instrucción no comprueba que el objeto esté accesible al jugador,
debe comprobarse con código.
```

```
doall <loc>

Ejecuta la frase actual tantas veces como objetos haya en la localidad, sustituyendo el nombre de la frase por el del objeto. Si durante este proceso sucediera un notdone, se cancelaría el resto de frases.
```