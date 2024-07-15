void display_message(const char *msg, bool log) {
    if (!DEBUGLOG_DISPLAY){
        if (!log){
            //print display message
            CoreS3.Display.drawString(msg);
            return;
        }
        else {
            return;
        }
    }
    else {
        //print display message
        CoreS3.Display.drawString(msg);
        return;  
    }
}

void button(const char *label, bool active) {
    
}
