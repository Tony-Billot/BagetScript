#pragma once
#include <string>

enum class TokenType {

    // Types de données
    TYPE_NOMBRE, // Nombre
    TYPE_TEXTE, // Chaine de caractères
    TYPE_BOOLEEN, // Booléens

    // Données en mémoire
    NOMBRE,
    TEXTE,
    VRAI,
    FAUX,

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
    ACCOLADE_GAUCHE,
    ACCOLADE_DROITE,
    POINT_VIRGULE,
    VIRGULE,

    // Instructions conditionnelles
    SI,
    SINON,
    SINON_SI,

    // Instructions d'affichage
    AFFICHER,

    // Opérateurs booléens
    EST_EGAL_A,
    EST_INFERIEUR_A,
    EST_SUPERIEUR_A,
    NON,
    ET,
    OU,

    // Mots réservés pour les fonctions
    FONCTION,
    RETOURNER,

    // Fin de fichier
    FIN_DE_FICHIER
};


struct Token
{
    TokenType type;
    std::string valeur;
    int line;
    int column;
};