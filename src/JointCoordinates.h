
#ifndef  JOINT_COORDINATES_H_
#define  JOINT_COORDINATES_H_
#include <math.h>
#include <string.h>

class JointCoordinates
{
    public:
        float x;
        float y;
        float z;
        char *name;
        const char *member_name;
        float lastAngle;
        float angle;
        float angle3D;
        bool available;
        bool dependenciesAvailable;

    public:
    
    /*!\brief Constructeur */
    JointCoordinates()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        angle = 0.0;
        lastAngle = 0.0;
        angle3D = 0.0;
        available               = false;
        dependenciesAvailable   = false;
    }
    
    /*!\brief Fonction setters qui attribue des coordonnées x, y et z  */
    void setCoordinates(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }
    
    /*!\brief Fonction qui retourne TRUE si le membre est disponible et FALSE dans le cas contraire */
    bool isAvailable()
    {
        return available;
    }
    
    /*!\brief Fonction qui  analyse si les membres dont dépent le membre courant sont disponible (nécéssaire pour le calcul de l'angle)*/
    bool areDependenciesAvailable(JointCoordinates* a, JointCoordinates* b)
    {
        if(a->available && available && b->available)
            return true;
        return false;
    }
    
    /*!\brief Fonction qui calcule l'angle entre trois membres (fonctionne uniquement en 2D) */
    void calculateAngle(JointCoordinates* a, JointCoordinates* c)
    {
        //Si l'un des points n'est pas disponible
        dependenciesAvailable = areDependenciesAvailable(a, c);
        if(dependenciesAvailable == false)
            return;
        
        JointCoordinates* ab = new JointCoordinates();
        JointCoordinates* cb = new JointCoordinates();
        
        ab->x = this->x - a->x ;
        ab->y = this->y - a->y ;
        
        cb->x = this->x - c->x ;
        cb->y = this->y - c->y ;
        
        float dot = (ab->x * cb->x + ab->y * cb->y);
        float cross = (ab->x * cb->y - ab->y * cb->x);
        float alpha = atan2(cross, dot);
        
        angle = floor(alpha * 180. / M_PI + 0.5);
    }
    
    /*!\brief Fonction qui calcule l'angle entre 3 points (fonctionne en 3D)*/
    double calculateAngle3D( double* a, double* c )
    {
        double ab[3] = { a[0] - x, a[1] - y, a[2] - z };
        double bc[3] = { c[0] - x, c[1] - y, c[2] - z };
        
        double abVec = sqrt(ab[0] * ab[0] + ab[1] * ab[1] + ab[2] * ab[2]);
        double bcVec = sqrt(bc[0] * bc[0] + bc[1] * bc[1] + bc[2] * bc[2]);
        
        double abNorm[3] = {ab[0] / abVec, ab[1] / abVec, ab[2] / abVec};
        double bcNorm[3] = {bc[0] / bcVec, bc[1] / bcVec, bc[2] / bcVec};
        
        double res = abNorm[0] * bcNorm[0] + abNorm[1] * bcNorm[1] + abNorm[2] * bcNorm[2];
        
        return acos(res)*180.0/ 3.141592653589793;
    }
    
    
    
    /*!\brief Fonction qui retourne le nom du membre */
    char* getName()
    {
        return name;
    }
};

extern JointCoordinates*  TETE           ;
extern JointCoordinates*  NUQUE          ;
extern JointCoordinates*  EPAULE_DROITE  ;
extern JointCoordinates*  EPAULE_GAUCHE  ;
extern JointCoordinates*  COUDE_GAUCHE   ;
extern JointCoordinates*  COUDE_DROIT    ;
extern JointCoordinates*  MAIN_GAUCHE    ;
extern JointCoordinates*  MAIN_DROITE    ;
extern JointCoordinates*  TORSE          ;
extern JointCoordinates*  HANCHE_GAUCHE  ;
extern JointCoordinates*  HANCHE_DROITE  ;
extern JointCoordinates*  GENOU_DROIT    ;
extern JointCoordinates*  GENOU_GAUCHE   ;
extern JointCoordinates*  PIED_DROIT     ;
extern JointCoordinates*  PIED_GAUCHE    ;
extern JointCoordinates*  BASE_COLONNE_V ;
















#endif
