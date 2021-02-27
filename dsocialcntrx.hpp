#include <arisen/arisen.hpp>
#include <string>

// CONSTANTS
const CONTRACT_NAME = name("dsocialcntrx");
const MAX_POST_SIZE = 140; // Posts can be a max of 140 bytes (characters);
const MAX_COMMENT_SIZE = 140;
const MAX_PROFILE_BIO_SIZE = 140; 
const MAX_PROFILE_URL_SIZE = 64; 
const MAX_PROFILE_DISPLAY_NAME_SIZE = 32; 
const MAX_PROFILE_LOCATION_SIZE = 64;
const DRIVE_URL_SIZE = 64;

class [[arisen::contract("dsocialcntrx")]]  dsocialcntrx : public contract {
  public:
    using contract::contract;
    using dwebhash_t = std::string;
   
    // HELPERS
    // TODO: ADD SHA256 FUNCTION FOR CREATING UNIQUE UUIDs BASED ON POST/COMMENT DATA
    
    static uint32_t timestamp() {
      return arisen::current_time_point().sec_since_epoch();
    };

    // ABI FUNCTIONS
    //@abi action
    [[arisen::action]]
    void post(name poster,
                    std::string data,
                    dwebhash_t drive);

     //@abi action
    [[arisen::action]]
    void modpost(uint64_t uuid,
                         std::string data,
                         dwebhash_t drive);

    //@abi action
    [[arisen::action]]
    void unpost(uint64_t uuid);

    //@abi action
    [[arisen::action]]
    void comment(name commenter,
                          uint64_t post_uuid,
                          std::string data,
                          dwebhash_t drive);

     //@abi action
     [[arisen::action]]
     void modcomment(uint64_t uuid,
                                 std::string data,
                                 dwebhash_t drive);
    //@abi action
    [[arisen::action]]
    void uncomment(uint64_t comment_uuid);

    //@abi action
    [[arisen::action]]
    void love(uint64_t uuid_pointer,
                   bool is_post,
                   name lover);

    //@abi action
    [[arisen::action]]
    void unlove(uint64_t lover,
                      uint64_t uuid_pointer);

    //@abi action
    [[arisen::action]]
    void repost(name reposter,
                      uint64_t post_uuid);

    //@abi action
    [[arisen::action]]
    void unrepost(uint64_t post_uuid,
                          name reposter);
    
    //@abi action
    [[arisen::action]]
    void upvote(uint64_t uuid,
                       uint64_t content_uuid,
                       name from,
                       name to,
                       arisen::asset payment_total);

    //@abi action
    [[arisen::action]]
    void favorite(uint64_t post_uuid,
                        name user);

    //@abi action
    [[arisen::action]]
    void unfavorite(uint64_t uuid);

    //@abi action
    [[arisen::action]]
    void pin(name pinner,
                 uint64_t content_uuid);

    //@abi action
    [[arisen::action]]
    void unpin(name pinner);

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
                    uint64_t content_uuid);

     //@abi action
     [[arisen::action]]
     void unhide(uint64_t content_uuid);

     //@abi action
     [[arisen::action]]
     void pdata(name user,
                      name display_name,
                      std::string url,
                      name bio,
                      name location);

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
                       std:string filename);

    //@abi action
    [[arisen::action]]
    void updatebio(name user,
                           name bio);

    //@abi action
    [[arisen::action]]
    void updateurl(name user,
                          std::string url);

    //@abi action
    [[arisen::action]]
    void updatename(name user,
                               name display_name);

    //@abi action
    [[arisen::action]]
    void updatelocation(name user,
                                  name location);

  private:
    // DATABASE SCHEMAS
    /// Post Schema 
    struct [[arisen::table]] postx {
      uint64_t uuid; 
      name poster; 
      std::string data; 
      dwebhash_t drive; 
      unit32_t timestamp; 
      
      //// Define Post Indices
      uint64_t primary_key()const { return uuid; }
      unit64_t get_post_by_user()const { return poster.value; }
    };

    /// Comment Schema
    struct [[arisen::table]] commentx {
      unit64_t comment_uuid; 
      name commenter; 
      uint64_t post_uuid; 
      std::string data; 
      dwebhash_t drive; 
      uint32_t timestamp;

      //// Define Comment Indices
      uint64_t primary_key()const { return comment_uuid; }
      uint64_t get_comment_by_post_uuid()const { return post_uuid; }
      uint64_t get_comment_by_user()const { return commenter.value; }
    };

    /// Love Schema
    struct [[arisen::table]] lovex {
      name lover; 
      uint64_t uuid_pointer; 
      bool is_post;

      //// Define Love Indices
      uint64_t primary_key()const { return lover.value; }
      uint64_t love_by_pointer()const { return uuid_pointer; }
    };

    /// User Schema
    struct [[arisen::table]] userx {
      name user; 
      name display_name;
      std::string url;
      name bio;
      name location;

      //// Define User Indices
      uint64_t primary_key()const { return user.value; }
      uint64_t user_by_bio()const { return bio.value; }
      uint64_t user_by_location() const {return location.value; }
    };

   /// Repost Schema
    struct [[arisen::table]] repostx {
      name reposter;
      uint64_t post_uuid;
      uint32_t timestamp;

      //// Define Repost Indices
      uint64_t primary_key()const { return post_uuid; }
      uint64_t repost_by_reposter()const { return reposter; }
    };

    /// Favorite Schema
    struct [[arisen::table]] favoritex {
      uint64_t uuid;
      uint64_t post_uuid;
      name user;

      //// Define Favorite Indices
      uint64_t primary_key()const { return uuid; }
      uint64_t fav_by_post_uuid()const { return post_uuid; }
    };

    /// Upvote Schema
    struct [[arisen::table]] upvotex {
      uint64_t uuid;
      name from;
      name to;
      arisen::asset payment_total;
      uint32_t timestamp;

      //// Define Upvote Indices
      uint64_t primary_key()const { return uuid; }
      uint64_t upvote_by_upvoter()const { return from.value; }
      uint64_t upvote_by_receiver()const { return to.value; }
    };

    /// Pin Schema
    struct [[arisen::table]] pinx {
      name pinner;
      uint64_t content_uuid;
      uint32_t timestamp;

      //// Define Pin Index
      uint64_t primary_key()const { return pinner.value; }
    };

    /// Hidden Schema
    struct [[arisen::table]] hiddenx {
      name hider;
      uint64_t content_uuid;
      uint32_t timestamp;

      //// Define Hidden Index
      uint64_t primary_key()const { return content_uuid; }
    };

    //// Followers Schema
    struct [[arisen::table]] followx {
      name follower;
      name following;

      //// Define Follower Indices
      uint64_t primary_key()const { return following.value; }
      uint64_t by_follower()const { return follower.value; }
    };

    //// Blocked Profiles Schema
    struct [[arisen::table]] blockx {
      name blocker;
      name blocked;

      //// Define Blocked Indices
      uint64_t primary_key()const { return blocked.value; }
      uint64_t by_blocker()const { return blocker.value; }
    };

    /// Profile Image Schema
    struct [[arisen::table]] pimagex {
      name uploader;
      dwebhash_t drive;
      std::string filename;

      //// Define Profile Image Index
      uint64_t primary_key()const { return uploader.value; }
    };

    //// Profile Header Schema
    struct [[arisen::table]] himagex {
      name uploader;
      dwebhash_t drive;
      std::string filename;

      //// Define Header Image Index
      uint64_t primary_key()const { return uploader.value; }
    };

    //// Account Status Schema
    struct [[arisen::table]] statusx {
      name user;
      
      //// Define Account Status Index
      uint64_t primary_key()const { return user.value; }
    };

    // DATABASE TABLES

    /// Post Table 
    using post_table = typedef arisen::multi_index<"posttbl"_n, postx,
                indexed_by<"get_post_by_user"_n, const_mem_fun<postx, uint64_t, &postx::get_post_by_user>>;

    /// Comment Table
    using comment_table = typedef arisen::multi_index<"commenttbl"_n, commentx,
                indexed_by<"get_comment_by_post_uuid"_n, const_mem_fun<commentx, uint64_t, &commentx::get_comment_by_post_uuid>>,
                indexed_by<"get_comment_by_user"_n, const_mem_fun<commentx, uint64_t, &commentx::get_comment_by_user>>;

    /// Love Table
    using love_table = typedef arisen::multi_index<"lovetbl"_n, lovex, 
                indexed_by<"love_by_pointer"_n, const_mem_fun<lovex, uint64_t, &lovex::love_by_pointer>>;

    /// User Table
    using user_table = typedef arisen::multi_index<"usertbl"_n, userx, 
                indexed_by<"user_by_bio"_n, const_mem_fun<userx, uint64_t, &userx::user_by_bio>>,
                indexed_by<"user_by_location"_n, const_mem_fun<userx, uint64_t, &userx::user_by_location>>;

    /// Repost Table
    using repost_table = typedef arisen::multi_index<"reposttbl"_n, repostx,
                indexed_by<"repost_by_reposter"_n, const_mem_fun<repostx, uint64_t, &repostx::repost_by_reposter>>;

    /// Favorite Table
    using favorite_table = typedef arisen::multi_index<"favoritetbl"_n, favoritex,
                indexed_by<"fav_by_post_uuid"_n, const_mem_fun<favoritex, uint64_t, &favoritex::fav_by_post_uuid>>;

    /// Upvote Table
    using upvote_table = typedef arisen::multi_index<"upvotetbl"_n, upvotex,
                 indexed_by<"upvote_by_upvoter"_n, const_mem_fun<upvotex, uint64_t, &upvotex::upvote_by_upvoter>>,
                 indexed_by<"upvote_by_receiver"_n, const_mem_fun<upvotex, uint64_t, &upvotex::upvote_by_receiver>>;

    /// Pin Table
    using pin_table = typedef arisen::multi_index<"pintbl"_n, pinx>;

    /// Hidden Posts Table
    using hidden_table = typedef arisen::multi_index<"hiddentbl"_n, hiddenx>;

    /// Follower Table
    using follower_table = typedef arisen::multi_index<"followertbl"_n, followerx,
                indexed_by<"by_follower"_n, const_mem_fun<followerx, uint64_t, &followerx::by_follower>>; 

    /// Blocked Users Table
    using block_table = typedef arisen::multi_index<"blocktbl"_n, blockx,
                indexed_by<"by_blocker"_n, const_mem_fun<blockx, uint64_t, &blockx::by_blocker>>;

    /// Profile Image Table
    using pimage_table = typedef arisen::multi_index<"pimagetbl"_n, pimagex>;

    /// Header Image Table
    using himage_table = typedef arisen::multi_index<"himagetbl"_n, himagex>;

    /// Account Status Table
    using status_table = typedef arisen::multi_index<"statustbl"_n, statusx>;
