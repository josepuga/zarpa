################################################################################
# Mostrar objetos de la localidad actual
#
# Código genérico para mostrar el contenido de una localidad al describirla. Si
# tuvieras restricciones en el jugador de tu aventura para ver los objetos.
# (oscuridad, etc.), tendrías que añadir código antes de esta sentencia.
#
# RECUERDA: en esta fase NO se tiene en cuenta lo tecleado por el jugador
# porque esta acción transcurre ANTES de que teclee algo.
################################################################################

: * *
	!empty _locPlayer		# Si no está vacía la loc., es que hay algún objeto
	msgln _msgObjectsHere  	# "Puedes ver...[loc:_locPlayer]"

