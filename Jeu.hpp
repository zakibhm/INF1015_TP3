#pragma once
#include <string>
#include "Liste.hpp"
#include "Concepteur.hpp"
//using ListeConcepteurs = Liste<Concepteur>;
using namespace std;
struct Jeu
{
	std::string titre;
	int anneeSortie;
	std::string developpeur;
	Liste<Concepteur> concepteurs;
};
