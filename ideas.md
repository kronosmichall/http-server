```
// Read the request
char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
sscanf(buffer, "%s %s %s", method, uri, version);
```