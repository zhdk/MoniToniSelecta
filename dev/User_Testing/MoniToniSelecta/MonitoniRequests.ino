///////////////////////------ permissen request ------///////////////////////

bool permissionRequest()
{ 
  if (!requestActive) {
    //timerServerTimeout.stop();
    //timerServerTimeout.start();
  }
  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    Serial.println("Connection failed");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_permission);
  client.println(String("GET " + url_permission + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(String("Failed to send request"));
    client.stop();
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0)
  {
    Serial.print(String("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(String("Invalid response"));
    client.stop();
    return 0;
  }

  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error)
  {
    Serial.print(String("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return 0;
  }

  // Extract Values
  permission = doc["HasPermission"].as<bool>();

  if (permission == true)
  {
    transactionActive = true;
    timerServerTimeout.stop(); 
    //timerPurchaseTimeout.stop();
    //timerPurchaseTimeout.start();
    Serial.println(F("Item:"));
    // Serial.println(doc["HasPermission"].as<bool>());
    Serial.println(doc["Item"].as<int>());
    item = doc["Item"].as<int>();
    // Disconnect
    client.stop();
    return 1;
  }
  else {
    transactionActive = false;
    //timerPurchaseTimeout.stop();
    // Disconnect
    client.stop();
    return 0;
  }
}

///////////////////////------ complete request ------///////////////////////

bool completeRequest()
{
  transactionActive = false;
  permission = false;

  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    Serial.println("Connection failed");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_complete);
  client.println(String("GET " + url_complete + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(String("Failed to send request"));
    client.stop();
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  /*
  Serial.println("");
  Serial.println("Request Completed");
  Serial.println("Received this from Server: ");
  Serial.println(status);
  Serial.println("");
  Serial.println("");
  */
  if (strcmp(status, "HTTP/1.1 201 Created") != 0)
  {
    Serial.print(String("Unexpected response: "));
    Serial.println(status);
    client.stop();
    Serial.println("UNSUCCESSFUL Request");
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(String("Invalid response"));
    Serial.println("UNSUCCESSFUL Request");
    client.stop();
    return 0;
  }

  // Disconnect
  client.stop();

  Serial.println("Successful Request");

  return 1;
}



///////////////////////------ close request ------///////////////////////

bool closeRequest()
{
  transactionActive = false;
  permission = false;

  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    Serial.println("Connection failed");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_close);
  client.println(String("GET " + url_close + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(String("Failed to send request"));
    client.stop();
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  /*
  Serial.println("");
  Serial.println("Request Completed");
  Serial.println("Received this from Server: ");
  Serial.println(status);
  Serial.println("");
  Serial.println("");
  */
  if (strcmp(status, "HTTP/1.1 201 Created") != 0)
  {
    Serial.print(String("Unexpected response: "));
    Serial.println(status);
    client.stop();
    Serial.println("UNSUCCESSFUL Request");
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(String("Invalid response"));
    Serial.println("UNSUCCESSFUL Request");
    client.stop();
    return 0;
  }

  // Disconnect
  client.stop();

  Serial.println("Successful Request");

  return 1;
}