In order to make vis unambiguous int he case of an input such as \X0A I propose encoding the "\" character itself as well.

When the "\" character is encoded as well, the output will be unambiguous, since the user will know that the only visible "\" characters in the output will be the ones outputted by the program itself, and that every "\" character in the output will always be followde by an X and 2 hex digits and nothing more.
