
# HTTP, Client-REST

The commands executed by the program are:

  *  register: Creates a JSON containing the username and password, which is then sent to the server via a POST request to check if the username is available for account creation or already taken. Based on the response, appropriate messages are displayed.

  *  login: Prompts the user for a username and password and sends a GET request to the server. If the user exists and the password is correct, the server returns a cookie, which serves as proof that the user is authenticated.

  * enter_library: Using the cookie that confirms the session with the server, the program attempts to obtain a token, which allows access to the user's library of books. If the user is not logged in, an error message is displayed.
		
  * get_books: If the user is logged in and has obtained the token for library access, a GET request can be sent to the server to retrieve and display all books in the current user's library.
		
  * add_book: The user is prompted to enter all the necessary details for a book (ensuring that all input fields are correctly filled).
		
  * get_book: Sends a GET request to the server for a book with the ID provided by the user. If no book with the given ID exists, an error message is displayed.
		
  * delete_book: Works similarly to get_book, with the only difference being that it sends a DELETE request instead of a GET request. If the book to be deleted does not exist, an appropriate message is displayed.
		
  * logout: Logs the user out of the server and clears the associated cookies and access token.
		
  * exit: Terminates the program execution (and frees dynamically allocated memory).

I chose to use the parson.h library from the GitHub repository provided in the assignment instructions. To create a JSON, I used the JSON_File and JSON_Value structures. A useful function from the library is json_serialize_to_string_pretty, which I used to display information in a well-formatted character string.
