/*==================================================================================
||                                                                                 ||
||      Projet B - Gestion de parkings à Lille - Langage C                         ||
||      Philippine Lambrechts - TD2 - TP3                                                   ||
||      Licence Mathématiques - UPHF                                               ||
||                                                                                 ||
===================================================================================*/ 


#include <stdio.h>
#include <string.h>

// ============================================================
// Projet : Gestion des parkings de la métropole lilloise
// Ce programme permet de gérer des parkings à partir d’un fichier CSV.
// Il propose plusieurs fonctionnalités : affichage, gestion des entrées/sorties,
// suivi des clients et mode administrateur.
// ============================================================


// Définitions des constantes pour les tailles des champs et le nombre maximum de parkings
#define TAILLE_ID 20
#define TAILLE_NOM 100
#define TAILLE_ADRESSE 150
#define TAILLE_VILLE 100
#define TAILLE_ETAT 20
#define TAILLE_DATE 40
#define MAX_PARKINGS 200

// Structure permettant de stocker toutes les informations d’un parking.
// Chaque ligne du fichier CSV correspond à une structure Parking.

typedef struct {
    char id[TAILLE_ID];
    char nom[TAILLE_NOM];
    char adresse[TAILLE_ADRESSE];
    char ville[TAILLE_VILLE];
    char etat[TAILLE_ETAT];
    int places_disponibles;
    int capacite;
    char date_maj[TAILLE_DATE];
    int places_occupees;
} Parking;

// Prototypes des fonctions utilisées dans le programme.
// Chaque fonction correspond à une tâche précise.


int lesparkings(const char *nomFichier, Parking parkings[], int *nbParkings);
int afficheparking(Parking parkings[], int nbParkings, const char *idRecherche);
int afficheparkings(Parking parkings[], int nbParkings);
float sortieParking(Parking parkings[], int nbParkings, const char *idParking, const char *plaque);
int lireEntierDansIntervalle(const char *message, int min, int max);
void initialiserFichierSuivi(const char *nomFichier);
int suiviClient(const char *nomFichier, const char *plaque, float montant, int hEntree, int mEntree, int hSortie, int mSortie);
void afficherSuiviClients(const char *nomFichier);
int mettreAJourOccupation(Parking parkings[], int nbParkings, const char *idParking, int entree);
int sauvegarderParkings(const char *nomFichier, Parking parkings[], int nbParkings);
int modeAdministrateur();
int verifierPlacesDisponibles(Parking parkings[], int nbParkings, const char *idParking);
void viderFichierSuivi(const char *nomFichier);

// Fonction principale du programme.
// Elle charge les données, initialise les fichiers,
// puis affiche un menu permettant à l’utilisateur d’interagir.


int main(void) {

    Parking parkings[MAX_PARKINGS];
    int nbParkings = 0;
    int choix;

// Lecture du fichier CSV contenant les parkings.
// Si le fichier ne peut pas être ouvert, le programme s’arrête.

    if (!lesparkings("parking-metropole.csv", parkings, &nbParkings)) {
        printf("Erreur: impossible de charger parking-metropole.csv\n");
        return 1;
    }

    initialiserFichierSuivi("suivi-clients.csv");

    printf("Chargement reussi: %d parkings trouves.\n", nbParkings);

// Boucle principale : le menu est affiché en continu
// jusqu’à ce que l’utilisateur choisisse de quitter.

    while (1) { // Affichage du menu et gestion des choix de l'utilisateur

        printf("\n============== MENU ==============\n");
        printf("1. Afficher un parking\n");
        printf("--------------------------------\n");
        printf("2. Afficher tous les parkings\n");
        printf("--------------------------------\n");
        printf("3. Entree d'un vehicule\n");
        printf("--------------------------------\n");
        printf("4. Sortie d'un vehicule\n");
        printf("--------------------------------\n");
        printf("5. Afficher suivi clients\n");
        printf("--------------------------------\n");
        printf("6. Mode administrateur\n");
        printf("--------------------------------\n");
        printf("7. Quitter\n");
        printf("==================================\n");

        choix = lireEntierDansIntervalle("Votre choix (1-7): ", 1, 7); // Lecture du choix de l'utilisateur avec validation

        if (choix == 1) { // Permet d’afficher les informations d’un parking à partir de son identifiant.

            char idRecherche[TAILLE_ID];

            printf("Entrez l'identifiant du parking: ");
            scanf("%19s", idRecherche);

            if (!afficheparking(parkings, nbParkings, idRecherche)) {
                printf("Parking introuvable.\n");
            }
        }

        else if (choix == 2) { // Affiche la liste complète des parkings disponibles.

            afficheparkings(parkings, nbParkings);
        }

        else if (choix == 3) { 

// Gestion de l’entrée d’un véhicule dans un parking :
// - vérification des places disponibles
// - mise à jour du nombre de places
// - sauvegarde dans le fichier CSV

            char idParking[TAILLE_ID];
            char plaque[20];

            printf("Entrez l'identifiant du parking: ");
            scanf("%19s", idParking);

            printf("Entrez la plaque du vehicule: ");
            scanf("%19s", plaque);
            
        if (verifierPlacesDisponibles(parkings, nbParkings, idParking) == 1) {
            printf("Parking plein !\n");
        } else {
            if (mettreAJourOccupation(parkings, nbParkings, idParking, 1)) {
                printf("Vehicule %s entre dans le parking.\n", plaque);
                sauvegarderParkings("parking-metropole.csv", parkings, nbParkings);
                printf("\nEtat du parking apres l'entree:\n");
                afficheparking(parkings, nbParkings, idParking);
            } else {
                printf("Parking introuvable.\n");
            }
        }
}


        else if (choix == 4) { 

// Gestion de la sortie d’un véhicule :
// - calcul du temps passé
// - calcul du prix
// - enregistrement dans le suivi clients
// - mise à jour du parking


            char idParking[TAILLE_ID];
            char plaque[20];
            float prix;

            printf("Entrez l'identifiant du parking: ");
            scanf("%19s", idParking);

            printf("Entrez la plaque du vehicule: ");
            scanf("%19s", plaque);

            prix = sortieParking(parkings, nbParkings, idParking, plaque); // Calcule le prix à payer et enregistre les données dans le suivi-clients.csv

            if (prix >= 0) { // Si le parking a été trouvé et le prix calculé

                printf("Paiement effectue: %.2f euros\n", prix);

                mettreAJourOccupation(parkings, nbParkings, idParking, 0);

                sauvegarderParkings("parking-metropole.csv", parkings, nbParkings);

                printf("\nEtat du parking apres la sortie:\n");
                afficheparking(parkings, nbParkings, idParking);
            }
        }

        else if (choix == 5) { // Affiche le contenu du fichier de suivi des clients.

            afficherSuiviClients("suivi-clients.csv");
        }

        else if (choix == 6) { // Mode administrateur : permet d’accéder à des fonctionnalités avancées.
            if (modeAdministrateur()) {

                printf("\n=== MODE ADMIN ===\n");
                printf("1. Modifier les places disponibles d'un parking\n");
                printf("2. Vider le fichier suivi-clients\n");

                int choixAdmin = lireEntierDansIntervalle("Votre choix (1-2): ", 1, 2);

                if (choixAdmin == 1) {
                    char id[TAILLE_ID];
                    int nouvellesPlaces;

                    printf("ID du parking: ");
                    scanf("%19s", id);
                    printf("Nouveau nombre de places: ");
                    scanf("%d", &nouvellesPlaces);

                    for (int i = 0; i < nbParkings; i++) {
                        if (strcmp(parkings[i].id, id) == 0) {
                            parkings[i].places_disponibles = nouvellesPlaces;
                            sauvegarderParkings("parking-metropole.csv", parkings, nbParkings);
                            printf("Modification effectuee.\n");
                            break;
                        }
                    }
                }

                else if (choixAdmin == 2) {
                    printf("Voulez-vous vider le fichier suivi-clients ? (1 = Oui / 2 = Non): ");
                    int confirmation = lireEntierDansIntervalle("", 1, 2);

                    if (confirmation == 1) {
                        viderFichierSuivi("suivi-clients.csv");
                    } else {
                        printf("Annulation.\n");
                    }
                }
            }
}


        else {

            printf("Fin du programme.\n");
            break;
        }
    }

    return 0;
}

int lesparkings(const char *nomFichier, Parking parkings[], int *nbParkings) { 


// Cette fonction lit le fichier CSV ligne par ligne
// et stocke les données dans un tableau de structures Parking.

    FILE *fichier = fopen(nomFichier, "r");
    char ligne[512];

    if (fichier == NULL) return 0;

// On ignore la première ligne du fichier (en-tête).

    fgets(ligne, sizeof(ligne), fichier);

    *nbParkings = 0;

    while (fgets(ligne, sizeof(ligne), fichier) && *nbParkings < MAX_PARKINGS) { // Lire chaque ligne du fichier et extraire les données

        Parking p;

// Extraction des données séparées par des points-virgules.

        int lus = sscanf(
            ligne,
            "%19[^;];%99[^;];%149[^;];%99[^;];%19[^;];%d;%d;%39[^;];%d",
            p.id, p.nom, p.adresse, p.ville, p.etat,
            &p.places_disponibles, &p.capacite, p.date_maj, &p.places_occupees
        );

        if (lus >= 6) { // Vérifier que les champs essentiels ont été lus

            if (lus < 9) {
                strcpy(p.date_maj, "inconnue"); // Valeur par défaut pour la date de mise à jour
                p.places_occupees = -1;
            }

            parkings[*nbParkings] = p;
            (*nbParkings)++;
        }
    }

    fclose(fichier);
    return 1;
}

int afficheparking(Parking parkings[], int nbParkings, const char *idRecherche) { // Recherche un parking par son identifiant et affiche ses informations.

    for (int i = 0; i < nbParkings; i++) {

        if (strcmp(parkings[i].id, idRecherche) == 0) {

            printf("\nID: %s\n", parkings[i].id);
            printf("Nom: %s\n", parkings[i].nom);
            printf("Ville: %s\n", parkings[i].ville);
            printf("Places disponibles: %d\n", parkings[i].places_disponibles);
            printf("Capacite: %d\n", parkings[i].capacite);

            return 1;
        }
    }

    return 0;
}

int afficheparkings(Parking parkings[], int nbParkings) { 

// Parcourt le tableau et affiche tous les parkings.

    for (int i = 0; i < nbParkings; i++) {

        printf("\n=== Parking %d ===\n", i + 1);
        printf("ID: %s\n", parkings[i].id);
        printf("Nom: %s\n", parkings[i].nom);
        printf("Places disponibles: %d\n", parkings[i].places_disponibles);
        printf("Capacite: %d\n", parkings[i].capacite);
    }

    return nbParkings;
}

float sortieParking(Parking parkings[], int nbParkings, const char *idParking, const char *plaque) { 

// Calcule le prix à payer lors de la sortie d’un véhicule en fonction du temps passé dans le parking.


    int hEntree, mEntree, hSortie, mSortie;

    for (int i = 0; i < nbParkings; i++) {

        if (strcmp(parkings[i].id, idParking) == 0) {

            printf("\nHeure d'arrivee\n");
            hEntree = lireEntierDansIntervalle("Heure (0-23): ",0,23);
            mEntree = lireEntierDansIntervalle("Minute (0-59): ",0,59);

            printf("\nHeure de sortie\n");
            hSortie = lireEntierDansIntervalle("Heure (0-23): ",0,23);
            mSortie = lireEntierDansIntervalle("Minute (0-59): ",0,59);

            int minutesEntree = hEntree*60 + mEntree; // Conversion des heures en minutes pour simplifier les calculs.
            int minutesSortie = hSortie*60 + mSortie;

            int duree = minutesSortie - minutesEntree;
            if (duree < 0) duree += 1440; // Si la sortie est après minuit, on corrige la durée.

            int heures = (duree + 59) / 60;// Arrondi à l’heure supérieure pour la facturation.

            float prix = heures * 2.0;

            printf("\nVehicule: %s\n", plaque);
            printf("Parking: %s\n", parkings[i].nom);
            printf("Duree: %d minutes\n", duree);
            printf("Montant a payer: %.2f euros\n", prix);

            suiviClient("suivi-clients.csv", plaque, prix, hEntree, mEntree, hSortie, mSortie);

            return prix;
        }
    }

    printf("Parking introuvable\n");
    return -1;
}

int lireEntierDansIntervalle(const char *message, int min, int max) { 

// Permet de sécuriser les entrées utilisateur en forçant un entier dans un intervalle donné.

    int valeur;

    while (1) {

        printf("%s", message);

        if (scanf("%d",&valeur) != 1) {
            while(getchar()!='\n');
            continue;
        }

        if (valeur < min || valeur > max) {
            printf("Valeur invalide\n");
            continue;
        }

        return valeur;
    }
}

void initialiserFichierSuivi(const char *nomFichier) { // Crée le fichier de suivi-clients.csv s'il n'existe pas et ajoute l'en-tête

    FILE *f = fopen(nomFichier,"r");

    if (f == NULL) {

        f = fopen(nomFichier,"w");

        if (f) {
            fprintf(f,"Plaque;Montant;HeureEntree;HeureSortie\n");
            fclose(f);
        }

        return;
    }

    fclose(f);
}

int suiviClient(const char *nomFichier, const char *plaque, float montant, int hE, int mE, int hS, int mS) { 

// Ajoute une ligne dans le fichier suivi-clients.csv avec les informations du client.

    FILE *f = fopen(nomFichier,"a");

    if (!f) return 0;

    fprintf(f,"%s;%.2f;%02d:%02d;%02d:%02d\n", plaque, montant, hE, mE, hS, mS);

    fclose(f);
    return 1;
}

void afficherSuiviClients(const char *nomFichier) { // Affiche tout le contenu du fichier de suivi.

    FILE *f = fopen(nomFichier,"r");
    char ligne[256];

    if (!f) {
        printf("Aucun fichier de suivi\n");
        return;
    }

    while (fgets(ligne,sizeof(ligne),f)) {
        printf("%s", ligne);
    }

    fclose(f);
}

int mettreAJourOccupation(Parking parkings[], int nbParkings, const char *idParking, int entree) { 

// Met à jour le nombre de places disponibles selon qu’un véhicule entre ou sort.

    for (int i = 0; i < nbParkings; i++) {

        if (strcmp(parkings[i].id, idParking) == 0) {

            if (entree) {

                if (parkings[i].places_disponibles > 0)
                    parkings[i].places_disponibles--;

            } else {

                if (parkings[i].places_disponibles < parkings[i].capacite)
                    parkings[i].places_disponibles++;
            }

            return 1;
        }
    }

    return 0;
}

int sauvegarderParkings(const char *nomFichier, Parking parkings[], int nbParkings) { // Réécrit entièrement le fichier CSV avec les nouvelles données.

    FILE *f = fopen(nomFichier,"w");

    if (!f) return 0;

    fprintf(f,"Identifiant;Nom;Adresse;Ville;Etat;Places disponibles;Capacite max;Date de mise a jour;Affichage panneaux\n");

    for (int i = 0; i < nbParkings; i++) {

        fprintf(
            f,
            "%s;%s;%s;%s;%s;%d;%d;%s;%d\n",
            parkings[i].id,
            parkings[i].nom,
            parkings[i].adresse,
            parkings[i].ville,
            parkings[i].etat,
            parkings[i].places_disponibles,
            parkings[i].capacite,
            parkings[i].date_maj,
            parkings[i].places_occupees
        );
    }

    fclose(f);
    return 1;
}

int modeAdministrateur() { // Vérifie si le code saisi correspond au code administrateur.

    char code[20];

    printf("Entrez le code administrateur: ");
    scanf("%19s", code);

    if (strcmp(code, "admin123") == 0) {
        printf("Acces autorise.\n");
        return 1;
    }

    printf("Code incorrect.\n");
    return 0;
}

int verifierPlacesDisponibles(Parking parkings[], int nbParkings, const char *idParking) { // Vérifie si un parking est plein ou non.

    for (int i = 0; i < nbParkings; i++) {

        if (strcmp(parkings[i].id, idParking) == 0) {

            if (parkings[i].places_disponibles == 0) {
                return 1;
            }

            return 0;
        }
    }

    return -1;
}

void viderFichierSuivi(const char *nomFichier) { // Vide le fichier de suivi en conservant l’en-tête.

    FILE *f = fopen(nomFichier, "w");

    if (!f) {
        printf("Erreur: impossible de vider le fichier.\n");
        return;
    }

    fprintf(f, "Plaque;Montant;HeureEntree;HeureSortie\n"); // On remet l'en-tête
    fclose(f);
    printf("Fichier suivi-clients remis a zero.\n");
}