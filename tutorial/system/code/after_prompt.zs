# version 0.0.1
# LOOK BUT DON'T TOUCH
# MIRA PERO NO TOQUES

###############################################################################
# Se tecleó un verbo incorrecto.
#
# Esta sentencia debe de ir al principio de esta tabla, ya que si no hay un verbo válido,
# no hay nada que se pueda hacer. Zarpa necesita mínimo un verbo en cada instrucción 
###############################################################################

: * *
	!voc _VERB				    # ¿NO es un verbo válido? (no existe en vocabulario)
    msgln _msgDontUnderstand    # "No comprendo..."   
    notdone


###############################################################################
# Salidas disponibles
#
# Lo normal en una aventura es que todas las localidades tengan almenos una
# salida.
# No obstante, puede ser interesante personalizar estos mensajes para situaciones
# específicas donde haya falta de luz, o quizás una especie de trampa en la
# que sólo se pueda salir de la localidad activando una secuencia en alguna
# parte. Para ello, usa variables o comandos como atloc.
###############################################################################
: EXIT *
    msgln _msgExits         # Mostrar dichas salidas
    done


###############################################################################
# Inventario del jugador
#
# Zarpa no tiene un comando "inv" en sí mismo, pero combinando los comandos
# msg y listat podemos listar los objetos de cualquier localidad,
# entidad u objeto
###############################################################################

# Aquí usamos "empty" para averiguar si una entidad, objeto o localidad tienen
# objetos. Como ejemplo voy a hacerlo de 2 maneras diferentes. Para los objetos
# llevados, uso la entidad jugador directamente, y zarpa buscará donde guarda
# sus objetos que es "_locCarried". Los objetos puestos, son un caso especial,
# ya que de momento el jugador es el único que puede hacerlo, uso empty con la
# localidad especial "_locWorn"

# Nótese que el done está puesto al final, para que se ejecute cualquier 
# combinación de llevado/puesto con/sin objetos 

: I *
    !empty _entPlayer           # igual que poner !empty _locCarried
    msgln _msgInvCarried        # "Tienes ...[loc:_locCarried]"
                                # No hay done, para que continúe con los puestos

: I *
    !empty _locWorn             # La localidad especial puestos no está vacía
    msgln _msgInvWorn           # "Llevas puesto ...[loc:_locWorn]"

: I *
    empty _locCarried           # Igual que poner empty _entPlayer
    msgln _msgInvCarriedNothing  # "No tienes nada."

: I *
    empty _locWorn              # La localidad especial de puestos está vacía
    msgln _msgInvWornNothing    # "No llevas nada interesante puesto."

: I *
    done


###############################################################################
# Examinar un objeto
#
###############################################################################

: LOOK *
    voc _OBJ                    # ¿Es un nombre de objeto válido?
    here _OBJ                   # ¿En la loc actual, llevado o puesto?
    exam _OBJ
    done    

: LOOK *
    voc _OBJ
    !here _OBJ                  # Es un objeto válido pero no está aquí
    msgln _msgCantSeeIt         # "No veo nada de eso."

: LOOK *
    !voc _OBJ                   # No es un objeto válido
    msgln _msgCantSeeIt         # "No veo nada de eso."


###############################################################################
# Movimiento genérico.
#
# Rutina estándar para el movimiento común del jugador.
#
# Si no se usara la forma "IR A <dirección>", no estaría la entrada ": GO *" y
# se podría simplificar todo con un simple 
# : * *
#   voc _MOV
#   (...) resto de acciones
###############################################################################

: GO *
: N *
: S *
: E *
: W *
: NE *
: SE *
: NW *
: SW *
: UP *
: DOWN *
    voc _MOV                    # ¿Verbo de movim. actual es válido? (no nulo)
    goto _MOV _entPlayer        # Hacer que el jugador se mueva
    desc                        # Describir la localidad
                                # A desc sólo se llega si se consigue movimiento
								# goto muestra el mensaje _msgCantGo "no puedo
								# ir ahí" y ejecuta un notdone si no hay salida

    
: GO *
: N *
: S *
: E *
: W *
: NE *
: SE *
: NW *
: SW *
: UP *
: DOWN *
    !voc _MOV
    msgln _msgCantDoIt        # "No puedes hacer eso..."
    notdone
 
    

################################################################################
# Dejar algo o todo
#
# Usamos doall que ejecuta el verbo actual (en este caso dejar) con todos los
# objetos de la localidad indicada.
#
# _OBJ = ID del objeto referido actual. Debe comprobarse con voc por si no
# hay ningún nombre encontrado
################################################################################

: DROP ALL
    empty _locCarried
    msgln _msgInvCarriedNothing
    notdone

: DROP ALL
    doall _locCarried			# Ejecuta DROP con todos los obj. de _locCarried
    done

: DROP *
    voc _OBJ                    # ¿Es un nombre de objeto válido?
    drop _OBJ _locPlayer        # Lo deja caer     
    msgln _msgDrop              # "Dejas..."
    done


################################################################################
# Coger algo o todo
#
# _OBJ = ID del objeto referido actual. Debe comprobarse con voc por si no
# hay ningún nombre encontrado
################################################################################

: GET ALL
    empty _locPlayer
    msgln _msgGetNothing
    notdone

: GET ALL
    doall _locPlayer
    done

: GET *
    voc _OBJ
    get _OBJ _locPlayer
    msgln _msgGet
    done



################################################################################
# Quitarse algo o todo
#
# El comando remove envía el mensaje _msgNotWorn y ejecuta un notdone si el 
# objeto referido no se lleva puesto. Sino, lo deja en llevados
#
# _OBJ = ID del objeto referido actual. Debe comprobarse con voc por si no
# hay ningún nombre encontrado
################################################################################

: REMOVE ALL
    empty _locWorn				# ¿Vacía la localidad de llevados?
    msgln _msgInvWornNothing	# "No llevas nada interesante puesto"
    notdone

: REMOVE ALL
    doall _locWorn				# Ver arriba DROP ALL
    done

: REMOVE *
    voc _OBJ					# ¿El nombre es de un objeto válido?
    remove _OBJ             	# Lo deja en llevados (ver cabecera para más info)
    msgln _msgRemove        	# "Te quitas..."
    done


################################################################################
# Ponerse algo o todo
#
# Hay que tener en cuenta que el comando "todo" fallará si hay algún objeto que 
# no sea ponible. Sólo se ponen los objetos que el jugador tenga, no los que
# estén en la localidad actual. Este comportamiento es así aposta.
#
# _OBJ = ID del objeto referido actual. Debe comprobarse con voc por si no
# hay ningún nombre encontrado
################################################################################

: WEAR ALL
    empty _locCarried
    msgln _msgInvCarriedNothing
    notdone 

: WEAR ALL
    doall _locCarried
    done

: WEAR *
    voc _OBJ
    wear _OBJ
    msgln _msgWear          # "Te pones..."
    done


################################################################################
# Código genérico cuando un objeto ni se puede poner/quitar/coger/Dejar. 
#
# Aunque ahorra código, los mensajes son menos personales. Ya que en vez de
# un "no puedo coger eso" o "no puedo ponermelo", sale un simple
# "no puedes hacer eso". Si decides tener más personalización, debes meter este
# código ligeramente modificado en cada entrada de poner/quitar/coger/Dejar
#
# _OBJ = ID del objeto referido actual. Debe comprobarse con voc por si no
# hay ningún nombre encontrado
################################################################################

# LLegados aquí, han fallado el resto de órdenes
: REMOVE *
: DROP *
    voc _OBJ                    # ¿Es un nombre de objeto válido?
    msgln _msgCantDoIt          # "No puedes hacer eso..."
    notdone

# Aquí se puede cambiar el mensaje a "no tengo eso" o "no entiendo"
# ya que el objeto no existe en la tabla de vocabulario   
: REMOVE * 
: DROP *
    !voc _OBJ                   # ¿Es un nombre de objeto NO válido?
    msgln _msgCantDoIt          # "No puedes hacer eso..."
    notdone


