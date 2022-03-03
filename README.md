# RPCUTools
Outils pour la RPCU

Il y a actuellement deux outils :

## RPCUPairing
A partir de deux jeux de données (données avant tranformation géométrique, données après transformation géométrique) reliés par un attribut permettant de faire le lien, RPCUPairing permet de générer :
* les vecteurs de déplacement entre les sommets, quand cela est pertinent
* les polygones avant / après, où les points de construction correspondent de manière bijective

## RPCUMover
A partir des polygones resultants de RPCUPairing, RPCUMover permet de transformer un jeu de données en appliquant l'algorithme de Haunert.
Dans le cadre de la RPCU, trois cas sont possibles :
* le point à transformer se trouve dans une parcelle : on applique l'algorithme de Haunert
* le point à tranformer se trouve à l'intérieur d'une feuille cadastrale mais dans le domaine non cadastré : on applique une triangulation à partir des points voisins
* le point est en dehors des feuilles cadastrales : on ne fait rien

