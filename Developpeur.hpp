#pragma once
#include <string>
#include "ListeJeux.hpp"
#include "Liste.hpp"
#include "Jeu.hpp"
class Developpeur
{
	//TODO: En faire une classe qui suit les principes OO.
	//TODO: La destruction d'un Developpeur doit s'assurer que la désallocation de ListeJeux est faite.
	//TODO: Les méthodes à faire...
public:
	Developpeur(const std::string& nom = "");
	void ajouterJeux(Liste<Jeu>& listeJeux);
	const std::string& getNom() const { return paireNomJeux_.first; }

	void afficher() const;

private:
	size_t compterJeuxDeveloppes(Liste<Jeu>& listeJeux);  // L'énoncé ne dit pas si ça doit être public ou non.

	std::pair<std::string, Liste<Jeu>> paireNomJeux_;
};
