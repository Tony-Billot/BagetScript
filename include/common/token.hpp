#include <string>


enum class TokenType {

    // Types de données
    TYPE_NOMBRE, // Nombre
    TYPE_TEXTE, // Chaine de caractères

    // Données en mémoire
    NOMBRE,
    TEXTE,

    // Idenfitiants
    IDENTIFIANT,

    // Opérateurs de base
    A_POUR_VALEUR,
    PLUS,
    MOINS,
    FOIS,
    DIVISE,

    // Délimiteurs, parenthèses, etc.
    PARENTHESE_GAUCHE,
    PARENTHESE_DROITE,
    POINT_VIRGULE,

    // Instructions d'affichage
    AFFICHER,

    // Fin de fichier
    FIN_DE_FICHIER,


};


struct Token
{
    TokenType type;
    std::string valeur;
};