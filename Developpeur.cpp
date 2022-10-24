#include "Developpeur.hpp"
#include "Jeu.hpp"
#include "Liste.hpp"
#include "main.cpp"
#include "cppitertools/range.hpp"
#include <iostream>
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.
using namespace std;

Developpeur::Developpeur(const string& nom)
{
	paireNomJeux_ = { nom, Liste<Jeu>{} };
}


void Developpeur::ajouterJeux(Liste<Jeu>& listeJeux){

	changerTailleListeJeux(paireNomJeux_.second, compterJeuxDeveloppes(listeJeux));
	for (size_t i : iter::range(listeJeux.nElements)) {
		if (listeJeux[i]->developpeur == getNom())
			ajouterJeu(paireNomJeux_.second, listeJeux[i]);
	}
}

void Developpeur::afficher() const
{
	cout << "\n" << getNom() << " a développé les jeux suivants :" << endl;
	if (paireNomJeux_.second.nElements > 0) 
	{
		for (size_t i : iter::range(paireNomJeux_.second.nElements)) {
			cout << "\t\033[33m" << paireNomJeux_.second[i]->titre << "\033[0m" << endl;
		}
	}
	else
		cout << "\t\033[31m" << "Aucun jeu trouvé, réessayez" << "\033[0m" << endl;
}

size_t Developpeur::compterJeuxDeveloppes(Liste<Jeu>& listeJeux)
{
	unsigned int n = 0;
	for (size_t i : iter::range(listeJeux.nElements)) {
		if (listeJeux[i]->concepteurs.elements[i]->nom == getNom())
			n++;
	}
	return n;
}
