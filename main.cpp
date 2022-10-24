#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include <memory>
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "ListeDeveloppeurs.hpp"
#include "debogage_memoire.hpp"//NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.
#include "Liste.hpp"
#include "Jeu.hpp"
#include "Concepteur.hpp"
#include "Developpeur.hpp"
using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}
#pragma endregion

//TODO: Fonction qui cherche un concepteur par son nom dans une ListeJeux.
// Cette fonction renvoie le pointeur vers le concepteur si elle le trouve dans
// un des jeux de la ListeJeux. En cas contraire, elle renvoie un pointeur nul.
shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, string nom)
{
	for (size_t i:range(listeJeux.nElements)){
		for (size_t j: range(listeJeux.nElements)) {
			if (nom == listeJeux[i]->concepteurs.elements[j]->nom)
				return listeJeux[i]->concepteurs.elements[j];
		}
	}
	return nullptr;
}


auto lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur = {}; // On initialise une structure vide de type Concepteur.
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);
	
	shared_ptr<Concepteur> concepteurExistant = trouverConcepteur(listeJeux, concepteur.nom);
	if (concepteurExistant != nullptr)
		return concepteurExistant;

	//cout << concepteur.nom << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
	std::cout << "\033[92m" << "Allocation en mémoire du concepteur " << concepteur.nom
				<< "\033[0m" << endl;
	return make_shared<Concepteur>(concepteur); //TODO: Retourner le pointeur vers le concepteur crée.
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux.
// Cette fonction doit recevoir en paramètre la nouvelle capacité du nouveau
// tableau. Il faut allouer un nouveau tableau assez grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit. N'oubliez pas, on copie
// des pointeurs de jeux. Il n'y a donc aucune nouvelle allocation de jeu ici !
void changerTailleListeJeux(Liste<Jeu>& liste, size_t nouvelleCapacite)
{
	assert(nouvelleCapacite >= liste.nElements); // On ne demande pas de supporter les réductions de nombre d'éléments.
	auto nouvelleListeJeux = make_unique<shared_ptr<Jeu>[]>(nouvelleCapacite);
	for (size_t i : iter::range(liste.nElements))
		nouvelleListeJeux[i] = move(liste[i]);

	liste.elements = move(nouvelleListeJeux);
	liste.capacite = nouvelleCapacite;
}

//TODO: Fonction pour ajouter un Jeu à ListeJeux.
// Le jeu existant déjà en mémoire, on veut uniquement ajouter le pointeur vers
// le jeu existant. De plus, en cas de saturation du tableau elements, cette
// fonction doit doubler la taille du tableau elements de ListeJeux.
// Utilisez la fonction pour changer la taille du tableau écrite plus haut.
void ajouterJeu(Liste<Jeu>& liste, shared_ptr<Jeu> jeu)
{
	if(liste.nElements == liste.capacite)
		changerTailleListeJeux(liste, max(size_t(1), liste.capacite * 2));  // En C++23, on peut utiliser 1uz au lieu du cast.
	liste[liste.nElements++] = jeu;
}

//TODO: Fonction qui enlève un jeu de ListeJeux.
// Attention, ici il n'a pas de désallocation de mémoire. Elle enlève le
// pointeur de la ListeJeux, mais le jeu pointé existe encore en mémoire.
// Puisque l'ordre de la ListeJeux n'a pas être conservé, on peut remplacer le
// jeu à être retiré par celui présent en fin de liste et décrémenter la taille
// de celle-ci.
void enleverJeu(Liste<Jeu>& liste, const shared_ptr<Jeu> jeu)
{
	for (size_t i : range(liste.nElements)) {
		if (liste[i] == jeu) {
			if (liste.nElements > 1)
				liste[i]= liste[liste.nElements - 1];
			liste.nElements--;
		}
	}
}

shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu = {}; // On initialise une structure vide de type Jeu
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	jeu.concepteurs.nElements = lireUintTailleVariable(fichier);
	shared_ptr<Jeu> ptrJeu = make_shared<Jeu>(jeu);
	std::cout << "\033[96m" << "Allocation en mémoire du jeu " << jeu.titre
			  << "\033[0m" << endl;
	ptrJeu->concepteurs.elements = make_unique<shared_ptr<Concepteur>[]>(jeu.concepteurs.nElements);
	for (size_t i : iter::range(ptrJeu->concepteurs.nElements)) {
		ptrJeu->concepteurs[i] = lireConcepteur(fichier, listeJeux);
		ajouterJeu(ptrJeu->concepteurs[i]->jeuxConcus, ptrJeu);
	}
	return ptrJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

Liste<Jeu> creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	Liste<Jeu> listeJeux;
	for([[maybe_unused]] size_t n : iter::range(nElements))
	{
		ajouterJeu(listeJeux, lireJeu(fichier, listeJeux)); //TODO: Ajouter le jeu à la ListeJeux.
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}


void afficherConcepteur(const Concepteur& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
			  << endl;
}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses concepteurs.
// Servez-vous de la fonction afficherConcepteur ci-dessus.
void afficherJeu(const Jeu& j)
{
	cout << "Titre : " << "\033[94m" << j.titre << "\033[0m" << endl;
	cout << "Parution : " << "\033[94m" << j.anneeSortie << "\033[0m"
			  << endl;
	cout << "Développeur :  " << "\033[94m" << j.developpeur << "\033[0m"
			  << endl;
	cout << "Concepteurs du jeu :" << "\033[94m" << endl;

	for (size_t i : range(j.concepteurs.nElements)) {
		afficherConcepteur(*j.concepteurs[i]);
	}
	cout << "\033[0m";
}

//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Servez-vous de la fonction d'affichage d'un jeu crée ci-dessus. Votre ligne
// de séparation doit être différent de celle utilisée dans le main.
void afficherListeJeux(const Liste<Jeu>& listeJeux)
{
	static const string ligneSeparation = "\n\033[95m"
	"══════════════════════════════════════════════════════════════════════════"
	"\033[0m\n";
	cout << ligneSeparation << endl;
	for (size_t i : range(listeJeux.nElements))
		afficherJeu(*listeJeux[i]);
		cout << ligneSeparation << endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi(); 
	#pragma endregion

	//int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	Liste<Jeu> lj = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	afficherJeu(*lj[0]);
	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	afficherListeJeux(lj);
	
	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	Liste<Developpeur> ld;
	// Création des développeurs externes à la liste des développeur
	shared_ptr<Developpeur> nintendo = make_shared<Developpeur>("Nintendo");
	shared_ptr<Developpeur> square = make_shared<Developpeur>("Square");
	shared_ptr<Developpeur> konami = make_shared<Developpeur>("Konami");
	shared_ptr<Developpeur> bidon = make_shared<Developpeur>("Bidon");
	// On ajoute les jeux respectifs de ListeJeux développé par le développeur
	nintendo->ajouterJeux(lj);
	square->ajouterJeux(lj);
	konami->ajouterJeux(lj);
	// On ajoute les développeurs à la ListeDeveloppeur car ils sont externes
	ld.ajouter(nintendo);
	ld.ajouter(square);
	ld.ajouter(konami);
	ld.ajouter(bidon);
	// On affiche la liste des développeurs, leurs jeux sont aussi affichés; Bidon ne devrait avoir aucun jeu.
	ld.afficher();

	cout << endl << "On retire " << bidon->getNom() << endl;
	//ld.retirer(bidon); // Retire sans détruire.
	ld.afficher();
	cout << "Il existe encore: " << bidon->getNom() << endl;

	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
}
