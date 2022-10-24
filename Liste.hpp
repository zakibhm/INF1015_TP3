#pragma once
#include <memory>
#include <cstddef>
#include <cassert>
#include "cppitertools/range.hpp"
#include "Developpeur.hpp"
#include "Jeu.hpp"
#include "Concepteur.hpp"
using namespace std;

template<typename T>
class Liste
{
public:
	Liste() = default;

	void ajouter(const shared_ptr<T> objet)
	{
		if (nElements == capacite)
			changerCapacite(max(size_t(1), capacite * 2));
		elements[nElements++] = move(objet);
	}

	auto operator[](size_t index) const {	return elements[index];}
	void afficher() const
	{
	}

	void changerCapacite(std::size_t nouvelleCapacite) {
		// Copie du code de changerTailleListeJeux, ajust� pour la classe.
		assert(nouvelleCapacite >= nElements); // On ne demande pas de supporter les r�ductions de nombre d'�l�ments.
		auto nouvelleListe = make_unique<shared_ptr<T>[]>(nouvelleCapacite);
		// Pas n�cessaire de tester si liste.elements est nullptr puisque si c'est le cas, nElements est n�cessairement 0.
		for (size_t i : iter::range(nElements))
			nouvelleListe[i] = move(elements[i]);

		elements = move(nouvelleListe);
		capacite = nouvelleCapacite;
	}  

	std::size_t nElements = 0, capacite = 0; 
	unique_ptr<shared_ptr<T>[]> elements;
	};