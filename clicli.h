#ifndef CLICLI_H                    // סימן התחלה – אם לא הגדרנו את CLICLI_H עדיין
#define CLICLI_H                    // מגדיר את CLICLI_H כדי שלא נטען פעמיים
#include "ev3lego.h"              // מחבר את הקובץ שמטפל ברובוט ובמנועים

class clicli {                    // מגדיר מחלקה בשם clicli
  private:
  ev3lego &mylego;               // משתנה שמצביע על הרובוט עצמו
  int number;                    // משתנה מספרי פנימי
  
  public: 
   clicli(ev3lego &mylego);      // פעולה שמפעילה את המחלקה עם חיבור לרובוט
   void begin();                 // פעולה שמריצים בתחילת התוכנית
   void run();                   // פעולה שמריצים כל הזמן בתוך loop()

};
#endif                           // סוף התנאי – סוגר את ההגדרה
