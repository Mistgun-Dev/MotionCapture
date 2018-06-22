#include "SceneContext.h"
#include "Window.h"
#include "Game.h"
#include "assert.h"
#include "Skybox.h"
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include "JointCoordinates.h"
#include "Logger.h"
#include <SDL2/SDL_ttf.h>
#include "SoundEffect.h"

float getAngleHands();
int score ;
const char* file[] = {"FBXLoader/Models/fly/export/AN-24PB_FBX.FBX",
                      "FBXLoader/Models/Coin/coin.fbx"};
TTF_Font *fontTTF = NULL;
SDL_Surface *message = NULL;
SceneContext *GameSceneContext = NULL;
SceneContext *CoinSceneContext = NULL;
GLfloat lumpos[4] = {1.0, 1.0, 1.0, 10.0};
pCoin dataCoins[NB_COINS];
static float coinRotation = 0.0;
int HeightPosition = 6;


GLuint quad;
GLuint messageId = 0;
int initSDL_TTF()
{
    quad = gl4dgGenQuadf();
    if( TTF_Init() == -1 ) {
        fprintf(stderr,"TTF ECHOUEEEE \n");
        return -1;
    }
    
    
    SDL_Color colorText = {255,255,255};
    //Ouverture du Font
    fontTTF = TTF_OpenFont("arial.ttf", 50);
    if( fontTTF == NULL ) {
        return -1;
    }
    //Mise en place du texte sur la surface message
    message = TTF_RenderText_Blended(fontTTF, "Bonjour", colorText);
    
    int colors = message->format->BytesPerPixel;
    int texture_format;
    
    glGenTextures(1, &messageId);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, messageId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, message->w, message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, message->pixels);
    
    return 0;
}


/*!\brief Fonction qui initialise le jeux, chargement des FBX, et des objets (piece, bombe ...)*/
int initGame()
{
    initSDL_TTF();
    score = 0;
    
    if((GameSceneContext = new SceneContext(file[0])) == NULL)
    {
        fprintf(stderr,"Initialisation de l'avion échoué !/n");
        return -1;
    }
    
    if((CoinSceneContext = new SceneContext(file[1])) == NULL)
    {
        fprintf(stderr,"Initialisation du FBX Coin échoué !/n");
        return -1;
    }
    
    if(initCoins() != 0)
    {
        fprintf(stderr,"Initialisation des données Coins échoué !/n");
        return -1;
    }
    
    return 0;
}

/*!\brief Fonction qui dessine l'avion */
void drawPlane()
{
    gl4duPushMatrix();
    
    if(HANCHE_GAUCHE->isAvailable() && HANCHE_DROITE->isAvailable())
    {
        glDisable(GL_BLEND);
        gl4duLoadIdentityf();
        gl4duTranslatef(0,-1, -6);
        gl4duScalef(0.1,0.1,0.1);
        gl4duRotatef(-90,0,1,0);
        gl4duRotatef(-10,0,0,1);
        //gl4duRotatef(BASE_COLONNE_V->angle, 1, 0, 0);
        gl4duRotatef(-getAngleHands(), 1, 0, 0);
        gl4duSendMatrices();
        glUniform1i(glGetUniformLocation(_pIdFBX, "myTexture"), 0);
        glUniform4fv(glGetUniformLocation(_pIdFBX, "lumpos"), 1, lumpos);
        GameSceneContext->OnDisplay();
       
    }
    
    //Import the scene if it's ready to load.
    if (GameSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)
    {
        GameSceneContext->LoadFile();
        //GameSceneContext->SetCurrentAnimStack(1);
    }
    
     gl4duPopMatrix();
}

/*!\brief Fonction qui dessine une pièces, en fonction de la position passéeen paramètre */
static void drawOneCoin(int x, int y, int z, int indice)
{
    glDisable(GL_BLEND);
    gl4duPushMatrix();
    gl4duTranslatef(x,y,z);
    gl4duScalef(1.3,1.3,1.3);
    gl4duRotatef(dataCoins[indice]->rotation,0,1,0);
    gl4duSendMatrices();
    glUniform1i(glGetUniformLocation(_pIdFBX, "myTexture"), 0);
    glUniform4fv(glGetUniformLocation(_pIdFBX, "lumpos"), 1, lumpos);
    
    if(dataCoins[indice]->id == COIN_ID)
    {
        CoinSceneContext->OnDisplay();
        if (CoinSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)
            CoinSceneContext->LoadFile();
    }
    
    else if(dataCoins[indice]->id == BOMB_ID)
    {
        CoinSceneContext->OnDisplay();
        if (CoinSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)
            CoinSceneContext->LoadFile();
    }

    dataCoins[indice]->rotation += 1.5;
    gl4duPopMatrix();
}

/*!\brief Fonction qui dessine toutes les pièces */
static void drawAllCoins()
{
    for(int i=0 ; i<NB_COINS ; i++)
    {
        if(dataCoins[i]->available == true)
            drawOneCoin(dataCoins[i]->posX , dataCoins[i]->posY , dataCoins[i]->posZ, i);
    }
}

/*!\brief Fonction qui renvoie un nombre aléatoire dans un interval donné */
int randomInt(int min, int max)
{
    //srand (time(NULL));
    return rand() % (max - min) + min + 1;
}

/*!\brief Fonction qui initialise le tableau de pièces en attribuants des positions aléatoires pour chaques pièces */
static int initCoins()
{
    int i, x;
    srand (time(NULL));
    
    for(i=0 ; i<NB_COINS ; i++)
    {
        dataCoins[i] = (pCoin)malloc(sizeof(Coin));
        if(dataCoins[i] == NULL)
            return -1;
        dataCoins[i]->available = false;
        dataCoins[i]->rotation = (float)randomInt(0,180);
    }

    //Initialisation des bombes
    for(i=0; i<NB_BOMB ; i++)
    {
        do
        {
            x = randomInt(0,NB_COINS-1);
        }
        while(dataCoins[x]->available == true);
       
        dataCoins[x]->id = BOMB_ID;
        dataCoins[x]->posX = randomInt(-_plan_r, _plan_r);
        dataCoins[x]->posY = HeightPosition;
        dataCoins[x]->posZ = randomInt(-_plan_r, _plan_r);
        dataCoins[x]->available = true;
    }
    
    //Initialisations des pieces
    for(i=0; i<NB_COINS ; i++)
    {
        if(dataCoins[i]->available == false)
        {
            dataCoins[i]->posX = randomInt(-_plan_r, _plan_r);
            dataCoins[i]->posY = HeightPosition;
            dataCoins[i]->posZ = randomInt(-_plan_r, _plan_r);
            dataCoins[i]->id = COIN_ID;
            dataCoins[i]->available = true;
        }
    }
    
    return 0;
}

/*!\brief Fonction qui définie l'orientation et la rotation de l'avion en fonction de l'angle de la colonne vertébrale */
void setOrientationPlane()
{
    int vitesse = 20;
    double dt, dtheta = M_PI;
    static Uint32 t0 = 0, t;
    dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
    t0 = t;
    
    if(HANCHE_GAUCHE->isAvailable() && HANCHE_DROITE->isAvailable() && MODE == GAME)
    {
        //Vitesse de l'avion
        _cam.x -= dt * vitesse * sin(_cam.theta);
        _cam.z -= dt * vitesse * cos(_cam.theta);
    }

    //Rotation theta de l'avion
    _cam.theta -= dt * -BASE_COLONNE_V->angle / 50;
}

/*!\brief Fonction qui teste si il y a une collision entre l'avion et une pièce */
static void testCollision()
{
    float mx,my,mz;
    float rayon = 8;
    
    for(int i=0 ; i<NB_COINS ; i++)
    {
        if(dataCoins[i]->available == true)
        {
            mx = (float)dataCoins[i]->posX;
            mz = (float)dataCoins[i]->posZ;
            if(_cam.x >= mx - rayon && _cam.x <= mx + rayon && _cam.z >= mz - rayon && _cam.z <= mz + rayon)
            {
                if(dataCoins[i]->id == COIN_ID || dataCoins[i]->id == BOMB_ID)
                {
                    dataCoins[i]->available = false;
                    playSound(SOUND_EFFECT_COIN, LOOP_ONCE);
                    score++;
                    fprintf(stderr,"SCORE = %d \n",score);
                }
                else
                    ;
            }
        }
    }
}


float getAngleHands()
{
    float centerX, centerY, diff;
    
    diff = (MAIN_DROITE->x - MAIN_GAUCHE->x) / 2;
    centerX = MAIN_GAUCHE->x + diff;
    
    diff = (MAIN_DROITE->y - MAIN_GAUCHE->y) / 2;
    centerY = MAIN_GAUCHE->y + diff;
    
    JointCoordinates *centre = new JointCoordinates();
    centre->available = true;
    centre->x = centerX;
    centre->y = centerY;
    centre->z = MAIN_DROITE->z;
    
    JointCoordinates *temp = new JointCoordinates();
    temp->available = true;
    temp->x = centre->x + 10;
    centre->y = centre->y;
    centre->z = MAIN_DROITE->z;
    
    centre->calculateAngle(temp, MAIN_DROITE);
    
    return centre->angle-90;
}

void printScore()
{
    SDL_Color colorText = {255,255,255};
    //Ouverture du Font
    fontTTF = TTF_OpenFont("arial.ttf", 50);
    if( fontTTF == NULL ) {
        ;
    }
    //Mise en place du texte sur la surface message
    message = TTF_RenderText_Blended(fontTTF, "Bonjour", colorText);
    
    int colors = message->format->BytesPerPixel;
    int texture_format;
    messageId = NULL;
    glGenTextures(1, &messageId);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, messageId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, message->w, message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, message->pixels);
    
    gl4duPushMatrix();
    gl4duLoadIdentityf();
    gl4duTranslatef(0,0,-10);
    gl4duScalef(1.5,0.4,0.4);
    gl4duRotatef(180,1,0,0);
    gl4duSendMatrices();
    glBindTexture(GL_TEXTURE_2D, messageId);
    gl4dgDraw(quad);
    gl4duPopMatrix();
}

/*!\brief Fonction d'affichage, qui affiche le contenue du mode jeux */
void drawGame()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //printScore();
    gl4duTranslatef(0,-HeightPosition,0);
    glUniform1i(glGetUniformLocation(_pIdFBX, "lightFBX"), 1);
    drawAllCoins();
    gl4duTranslatef(0,0,-10);
    drawPlane();
    testCollision();
    glUniform1i(glGetUniformLocation(_pIdFBX, "lightFBX"), 0);
    drawSkybox();
}

/*!\brief Fonction de libération de la mémoire du mode jeux */
void freeGame()
{
    LOG4CXX_INFO(GlobalLogger, "Liberation du mode jeu !");
    if(GameSceneContext)
        delete GameSceneContext;
    if(CoinSceneContext)
        delete CoinSceneContext;
    
    for(int i=0 ; i<NB_COINS ; i++)
        free(dataCoins[i]);

}
