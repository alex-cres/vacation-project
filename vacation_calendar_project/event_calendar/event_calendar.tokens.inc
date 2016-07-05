<?php

/**
 * @file
 * Builds placeholder replacement tokens for event_calendar-related data.
 */

/**
 * Implements hook_token_info().
 */
function event_calendar_token_info()
{
    $type = array(
        'name' => t('event_calendar'),
        'description' => t('Tokens related to individual content items, or "event_calendars".'),
        'needs-data' => 'event_calendar'
    );
    
    // Core tokens for event_calendars.
    $event_calendar['start_date']   = array(
        'name' => t("Event start date"),
        'description' => t('The start date of events in event_calendar.')
    );
    $event_calendar['end_date']     = array(
        'name' => t("Event end date"),
        'description' => t('The start date of events in event_calendar.')
    );
    $event_calendar['event_status'] = array(
        'name' => t("Event status"),
        'description' => t('The Status of events in event_calendar')
    );
    $event_calendar['approval_url'] = array(
        'name' => t("Event approval URL"),
        'description' => t('The URL for admin to approve events')
    );
    $event_calendar['total_days']   = array(
        'name' => t("Days"),
        'description' => t('The number of days that the date occupies')
    );
    
    $event_calendar['working_days'] = array(
        'name' => t("Days"),
        'description' => t('The number of days that are working days')
    );
    
    $event_calendar['holi_days'] = array(
        'name' => t("Holidays"),
        'description' => t('The date of days that are holidays')
    );
    
    return array(
        'types' => array(
            'event_calendar' => $type
        ),
        'tokens' => array(
            'event_calendar' => $event_calendar
        )
    );
}

/**
 * Implements hook_tokens().
 */
function event_calendar_tokens($type, $tokens, array $data = array(), array $options = array())
{
    $url_options = array(
        'absolute' => TRUE
    );
    if (isset($options['language'])) {
        $url_options['language'] = $options['language'];
        $language_code           = $options['language']->language;
    } else {
        $language_code = NULL;
    }
    $sanitize = !empty($options['sanitize']);
    
    $replacements = array();
    global $base_path;
    if ($type == 'event_calendar' && !empty($data['event_calendar'])) {
        $event_calendar  = $data['event_calendar'];
        //total_days , working_days , holi_days calculus
        $field_lang_code = field_language('node', $event_calendar, 'event_calendar_date', LANGUAGE_NONE);
        $start           = new DateTime($event_calendar->event_calendar_date[$field_lang_code]['0']['value']);
        $start->sub(new DateInterval('PT' . $start->format("H") . 'H' . $start->format("i") . 'M' . $start->format("s") . 'S'));
        $end = new DateTime($event_calendar->event_calendar_date[$field_lang_code]['0']['value2']);
        $end->sub(new DateInterval('PT' . $end->format("H") . 'H' . $end->format("i") . 'M' . $end->format("s") . 'S'));
        
        // otherwise the  end date is excluded (bug?)
        $end->modify('+1 day');
        $interval  = $end->diff($start);
        // total days
        $days      = $interval->days;
        $day_total = $days;
        // create an iterateable period of date (P1D equates to 1 day)
        $period    = new DatePeriod($start, new DateInterval('P1D'), $end);
        // best stored as array, so you can add more than one
        $query     = db_select('field_data_event_calendar_status', 'ecs');
        $query->join('taxonomy_term_data', 'td', 'td.tid = ecs.event_calendar_status_tid');
        $query->join('field_data_event_calendar_date', 'ecs2', 'ecs2.entity_id = ecs.entity_id');
        $query->fields('ecs2', array(
            'event_calendar_date_value'
        ))->condition('td.name', 'holiday');
        $result = $query->execute();
        $holi   = "";
        foreach ($result as $nod) {
            $holidays[] = $nod->event_calendar_date_value;
        }
        foreach ($period as $dt) {
            $dan  = false; //used to see if a holiday is met so it doesn't subtract a holiday in a sunday or saturday
            $curr = $dt->format('D');
            // for the updated question
            foreach (@$holidays as $noe) {
                if ($dt->format("Y-m-d H:i:s") == $noe) {
                    $holi .= "[" . $dt->format("Y-m-d") . "]";
                    $days--;
                    $dan = true;
                }
            }
            // substract if Saturday or Sunday
            if (($curr == 'Sat' || $curr == 'Sun') && !($dan)) {
                $days--;
            }
        }
        
        foreach ($tokens as $name => $original) {
            
            switch ($name) {
                // Simple key values on the event_calendar.
                case 'start_date':
                    $field_lang_code         = field_language('node', $event_calendar, 'event_calendar_date', LANGUAGE_NONE);
                    $replacements[$original] = $event_calendar->event_calendar_date[$field_lang_code]['0']['value'];
                    break;
                
                case 'end_date':
                    $field_lang_code         = field_language('node', $event_calendar, 'event_calendar_date', LANGUAGE_NONE);
                    $replacements[$original] = $event_calendar->event_calendar_date[$field_lang_code]['0']['value2'];
                    break;
                
                
                
                case 'total_days':
                    $replacements[$original] = $day_total;
                    break;
                
                case 'working_days':
                    
                    $replacements[$original] = $days;
                    break;
                
                case 'holi_days':
                    
                    $replacements[$original] = $holi;
                    break;
                
                
                case 'event_status':
                    $field_lang_code         = field_language('node', $event_calendar, 'event_calendar_status', LANGUAGE_NONE);
                    $term_load               = taxonomy_term_load($event_calendar->event_calendar_status[$field_lang_code]['0']['tid']);
                    $replacements[$original] = $term_load->name;
                    break;
                
                case 'approval_url':
                    $replacements[$original] = url("vacation_calendar/manage-events", $url_options);
                    break;
            }
        }
    }
    
    return $replacements;
}
