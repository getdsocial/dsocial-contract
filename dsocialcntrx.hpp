#include <arisen/arisen.hpp>
#include <string>

// CONSTANTS
const MAX_POST_SIZE = 140;
const DRIVE_URL_SIZE = 32;
const TOKEN_CONTRACT = name("dsocialliket");
const MAX_PROFILE_BIO_SIZE = 140;
const MAX_PROFILE_URL_SIZE = 64;
const MAX_PROFILE_DISPLAY_NAME_SIZE = 32;
const CONTRACT_NAME = name("dsocialcntrx");

class [[arisen::contract("dsocialcntrx")]] dsocialcntrx : public contract {
  public:
    using contract::contract;
    using dwebhash_t = std::string;

    // HELPERS
    static uint32_t timestamp() {
      return arisen::current_time_point().sec_since_epoch();
    };

    // DATABASE SCHEMAS
    /// Post Schema
    struct [[arisen::table]] postx {
      uint64_t uuid;
      name poster;
      std::string data;
      dwebhash_t drive;
      uint64_t in_reply_to;
      uint32_t timestamp;

      uint64_t primary_key()const { return uuid; }
      uint64_t get_post_by_user()const { return poster.value; }
      uint64_t get_by_reply()const { return in_reply_to; }
    };

    struct [[arisen::table]] lovex {
      name lover;

      uint64_t primary_key()const { return lover.value; }
    };

    struct [[arisen::table]] userx {
      name user;
      name display_name; 
      std::string url;
      std::string bio;
      std::string location;
      dwebhash_t hi_drive;
      dwebhash_t pi_drive;
      std::string pfn;
      std::string hfn;

      uint64_t primary_key()const { return user.value; }
      uint64_t user_by_dn()const {return display_name.value }
    };

    struct [[arisen::table]] repostx {
      uint64_t post_uuid;
      uint32_t timestamp;

      uint64_t primary_key()const { return post_uuid; }
    };

    struct [[arisen::table]] favoritex {
      uint64_t post_uuid;
     
      uint64_t primary_key()const { return post_uuid; }
    };

    struct [[arisen::table]] upvotex {
      name from;
      arisen::asset amount;
      uint32_t timestamp;

      uint64_t primary_key()const { return from.value; }
      uint64_t upvote_by_r()const { return to.value; }
    };

    struct [[arisen::table]] pinx {
      name pinner;
      uint64_t post_uuid;
      uint32_t timestamp;
 
      uint64_t primary_key()const { return pinner.value; }
    };

    struct [[arisen::table]] hiddenx {
      uint64_t post_uuid;
      uint32_t timestamp;

      uint64_t primary_key()const { return post_uuid; }
    };

    struct [[arisen::table]] followx {
      name following;

      uint64_t primary_key()const { return following.value; }
    };

    struct [[arisen::table]] blockx {
      name blocked;
      
      uint64_t primary_key()const { return user.value; }
    };

    struct [[arisen::table]] statusx {
      name user;
      
      uint64_t primary_key()const { return user.value; }
    };

    // DATABASE TABLES

    using post_table = typedef arisen::multi_index<"posttbl"_n, postx, indexed_by<"get_post_by_user"_n, const_mem_fun<postx, uint64_t, &postx::get_post_by_user>>, indexed_by<"get_by_reply"_n, const_mem_fun<postx, uint64_t, &postx::get_by_reply>> > posttbl;

    using love_table = typedef arisen::multi_index<"lovetbl"_n, lovex > lovetbl;

    using user_table = typedef arisen::multi_index<"usertbl"_n userx, indexed_by<"user_by_dn"_n, const_mem_fun<userx, uint64_t, &userx::user_by_dn>> > usertbl;

    using repost_table = typedef arisen::multi_index<"reposttbl"_n, repostx > reposttbl;
    using favorite_table = typedef arisen::multi_index<"favoritetbl"_n, favoritex > favoritetbl;
    using upvote_table = typedef arisen::multi_index<"upvotetbl"_n, upvotex, indexed_by<"upvote_by_r"_n, const_mem_fun<upvotex, uint64_t, &upvotex::upvote_by_r >> > upvotetbl;

    using pin_table = typedef arisen::multi_index<"pintbl"_n, pinx > pintbl;
    using hidden_table = typedef arisen::multi_index<"hiddentbl"_n, hiddenx > hiddentbl;

    using follow_table = typedef arisen::multi_index<"followtbl"_n, followx > followtbl;
    using block_table = typedef arisen::multi_index<"blocktbl"_n, blockx > blocktbl;
    using status_table = typedef arisen::multi_index<"statustbl"_n, statusx > statustbl; 

    // ABI Functions
    //@abi action
    [[arisen::action]]
    void post(name poster,
                   std::string data,
                   dwebhash_t drive,
                   uint64_t in_reply_to);

    //@abi action
    [[arisen::action]]
    void modpost(uint64_t post_uuid,
                         std::string data,
                         dwebhash_t drive);

    //@abi action
    [[arisen::action]]
    void unpost(uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void love(uint64_t uuid_pointer,
                   name lover);

    //@abi action
    [[arisen::action]]
    void unlove(uint64_t uuid_pointer,
                      name lover);

    //@abi action
    [[arisen::action]]
    void repost(name reposter,
                      uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void unrepost(name reposter,
                          uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void upvote(name from,
                       name to,
                       arisen::asset amount);

    //@abi action
    [[arisen::action]]
    void pin(name pinner,
                 uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void unpin(name pinner,
                     uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void favorite(uint64_t post_uuid,
                         name user);

    //@abi action
    [[arisen::action]]
    void unfavorite(uint64_t post_uuid,
                            name user);

    //@abi action
    [[arisen::action]]
    void follow(name follower,
                     name following);

     //@abi action
     [[arisen::action]]
     void unfollow(name follower,
                          name following);

     //@abi action
     [[arisen::action]]
     void block(name blocker,
                     name blocked);

     //@abi action
     [[arisen::action]]
     void unblock(name blocker,
                         name blocked);

     //@abi action
     [[arisen::action]]
     void hide(name hider,
                    uint64_t post_uuid);
  
     //@abi action
     [[arisen::action]]
     void unhide(name hider,
                       uint64_t post_uuid);

     //@abi action
     [[arisen::action]]
     void activate(name user);
  
     //@abi action
     [[arisen::action]]
     void deactivate(name user);

     //@abi action
     [[arisen::action]]
     void pimage(name uploader,
                        dwebhash_t drive,
                        std::string filename);

     //@abi action 
     [[arisen::action]]
     void himage(name uploader,
                        dwebhash_t drive,
                        std::string filename);

     //@abi action
     [[arisen::action]]
     void updatedn(name user,
                           std::string display_name);

     //@abi action
     [[arisen::action]]
     void updatelocation(name user,
                                   std::string location);

     //@abi action
     [[arisen::action]]
     void updateurl(name user,
                           std::string url);

     //@abi action
     [[arisen::action]]
     void updatebio(name user,
                            std::string bio);

}
