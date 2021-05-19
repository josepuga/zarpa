# IMPORTANTE: ESPACIO ENTRE LOS 2 PUNTOS ":" Y EL ID DEL VERBO

# ¿se ha tecleado empezar o empieza?
: EMPEZAR *                        
    atloc locStart _entPlayer # ¿Está en locStart la entidad jugador?
    moveto locParada _entPlayer # Moverse a la localidad locParada
    desc                      # Hacer descripción de la misma
                              # En realidad desc no hace una descripción, sino
                              # que hace un "goto" a la tabla de Zarpascript
                              # code/before_desc.zs. Pero esto lo veremos en el
                              # siguiente tutorial.

# Cualquier cosa tecleada
: * * 
    atloc locStart _entPlayer # ¿Está en locStart la entidad jugador?
    desc 