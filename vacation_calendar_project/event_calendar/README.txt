MODULE
------
 Vacation Calendar

Contents of files:
------------------

  * Requirements
  * Description/Features
  * Installation
  * Credits


REQUIREMENTS
------------
Drupal 7.0


DESCRIPTION/FEATURES
--------------------

 This module gives the possibility of managing a Holiday schedule for a company, 
 it also gives an Admin UI to change status (approval/denied) of events in the 
 Management Menus and a mailing system integrated inside the view.

 
Features
----------

 *  A mailing integration for approval/denial of event.
 *  Different colors of events based on their status.
 *  Vacation creation.
 *  Counter for used days.
 *  Submission and Cancel.

 
 
Benefits
----------
.
 * Easy to add events (if a user permits).
 * Events status can be easily identified with the color of events.
 * Easy interface.

 
Dependency
----------
  As it uses calendar format for the creation of “event calendar view” so it
  depends on the Calendar module. If you want to use dates in pop-up then it
  requires Date-pop also.
 
  * Calendar => http://drupal.org/project/calendar .
  * Date => http://drupal.org/project/date .
  * Date Popup(included with Date).
  * Relation => https://www.drupal.org/project/relation .
  * PHP(included in the Core)
  *Help(included in the Core).
  






Installation:
-------------
1. Copy Vacation Calendar folder to modules (usually 'sites/all/modules')
   directory.
2. Activate the module (it is not compatible with Event Calendar).
3. Import the view from the Vacation Calendar Others menu in [your site]/vacation_calendar/event-settings
4. Configure the Permissions for the various accesses.
5. Configure if needed the chiefs for the users, it defaults to the admin user.




CREDITS
--------



* This module was created by OSSCube Solutions Pvt Ltd. 
* Guided by Bhupendra Singh. 
* Developed by Radhey Shyam and Navneet Kumar. 
* It was further continued by Alexandre Realinho.
