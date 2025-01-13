
struct{
  double amm1 = -1;
  double amm2 = -2;
  double amm3 = -4;
  double amm4 = -5;
  double amm5 = -10;
  } Ammeter_readings;

struct{
  char batt1[10] = "3123.45";
  char batt2[10] = "3123.45";
  char batt3[10] = "3123.45";
  char batt4[10] = "3123.45";
  char batt5[10] = "3123.45";
} Batts_char;
char test[10] = {"abcdef"};

struct{
  double batt1;
  double batt2;
  double batt3;
  double batt4;
  double batt5;
} Batts_double;

void setupFile(fs::FS &fs, const char * path){
  Serial.printf("Writng file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for setup");
    return;
  }
  file.println(Batts_char.batt1);
  file.println(Batts_char.batt2);
  file.println(Batts_char.batt3);
  file.println(Batts_char.batt4);
  file.println(Batts_char.batt5);

  file.close();
  Serial.println("File has been setup, all states 100");
}

void writeFile(fs::FS &fs, const char * path){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  file.println(Batts_char.batt1);
  file.println(Batts_char.batt2);
  file.println(Batts_char.batt3);
  file.println(Batts_char.batt4);
  file.println(Batts_char.batt5);

  file.close();
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }else{
    Serial.println("file opened for reading");
  }

  Serial.println(Batts_char.batt1);
  file.seek(0);
  while(file.available()){
    int i = 0;
    char c = file.read();
    if(c == '\n'){
      break;
    }else{
      Batts_char.batt1[i] = c;
    }
    i++;
  }
  Serial.println("copy into batt 1 done");
  Serial.println(Batts_char.batt1);

  file.seek(6);
  while(file.available()){
    int i = 0;
    char c = file.read();
    if(c == '\n'){
      break;
    }else{
      Batts_char.batt2[i] = c;
    }
  }
  Serial.println("copy into batt 2 done");
  Serial.println(Batts_char.batt2);

  file.seek(12);
  while(file.available()){
    int i = 0;
    char c = file.read();
    if(c == '\n'){
      break;
    }else{
      Batts_char.batt3[i] = c;
    }
  }

  file.seek(18);
  while(file.available()){
    int i = 0;
    char c = file.read();
    if(c == '\n'){
      break;
    }else{
      Batts_char.batt4[i] = c;
    }
  }

  file.seek(24);
  while(file.available()){
    int i = 0;
    char c = file.read();
    if(c == '\n'){
      break;
    }else{
      Batts_char.batt5[i] = c;
    }
  }

}

void printchar(const char * message){
  Serial.printf(message);
  Serial.printf(Batts_char.batt1); Serial.print("  ");
  Serial.printf(Batts_char.batt2); Serial.print("  ");
  Serial.printf(Batts_char.batt3); Serial.print("  ");
  Serial.printf(Batts_char.batt4); Serial.print("  ");
  Serial.printf(Batts_char.batt5); Serial.println("  ");
}

void printdub(const char * message){
  Serial.printf(message);
  Serial.print(Batts_double.batt1); Serial.print("  ");
  Serial.print(Batts_double.batt2); Serial.print("  ");
  Serial.print(Batts_double.batt3); Serial.print("  ");
  Serial.print(Batts_double.batt4); Serial.print("  ");
  Serial.print(Batts_double.batt5); Serial.println("  ");
}
