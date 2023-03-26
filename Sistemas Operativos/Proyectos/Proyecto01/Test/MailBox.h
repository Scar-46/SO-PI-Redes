/**
  *   C++ class to encapsulate Unix message passing intrinsic structures and system calls
  *
 **/

class MailBox {
   public:
      MailBox();
      ~MailBox();
      int send( long, void * buffer, int capacity );
      int receive( long, void * buffer, int capacity );

   private:
      int id;		// mailbox id

};

