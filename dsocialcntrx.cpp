#include "dsocialcntrx.hpp"

[[arisen::action]]
void post(name poster,
               std::string data,
               dwebhash_t drive,
               uint64_t in_reply_to) {

  require_auth(poster);
  post_table posttable(get_self(), get_self().value);

  arisen::check(data.size() <= MAX_POST_SIZE, "Post must be 140 characters or less.");
  arisen::check(drive.size() == DRIVE_URL_SIZE, "Drive URL must be 64 hexadecimal characters.");

  // Check replied to post
  auto postitr = posttable.find(in_reply_to);
  arisen::check(postitr != posttable.end() || in_reply_to == 0, "You cannot reply to a post that does not exist.");

  posttable.emplace(get_self(), [&]( auto& p ) {
    p.uuid = posttable.available_primary_key();
    p.poster = poster;
    p.data = data;
    p.drive = drive;
    p.in_reply_to = in_reply_to;
    p.timestamp = timestamp();
  });
};

[[arisen::action]]
void modpost(uint64_t post_uuid,
                     std::string data,
                     dwebhash_t drive) {
  post_table posttable(get_self(), get_self().value);
  arisen::check(data.size() <= MAX_POST_SIZE, "Post must be 140 characters or less.");
  arisen::check(drive.size() == DRIVE_URL_SIZE, "Drive URL must be 64 hexadecimal characters");

  auto postitr = posttable.find(post_uuid);
  if (postitr != posttable.end()) {
    require_auth(postitr->poster);
    posttable.modify(postitr, get_self(), [&]( auto& row ) {
      row.data = data;
      row.drive = drive;
      row.timestamp = timestamp();
    });

  } else {
    arisen::check(false, "You cannot modify a post that does not exist.");
  };
};

[[arisen::action]]
void unpost(uint64_t post_uuid) {
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  arisen::check(postitr != posttable.end(), "You cannot delete a post that does not exist.");
  require_auth(postitr->poster);
  posttable.erase(postitr);
};

[[arisen::action]]
void love(uint64_t uuid_pointer,
               name lover) {

  require_auth(lover);
  love_table lovetable(get_self(), uuid_pointer);
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(uuid_pointer);

  if (postitr != posttable.end()) {
    lovetable.emplace(get_self, [&]( auto& l ) {
      l.lover = lover;
    });
  } else {
    arisen::check(false, "You cannot love a post that does not exist.");
  };
};

[[arisen::action]]
void unlove(uint64_t uuid_pointer,
                  name lover) {
  love_table lovetable(get_self(), uuid_pointer);
  auto loveitr = lovetable.find(lover);
  
  if (loveitr != lovetable.end()) {
    require_auth(loveitr->lover);
    lovetable.erase(loveitr);
  } else {};

};

[[arisen::action]]
void repost(name reposter,
                  uint64_t post_uuid) {

  require_auth(reposter);
  repost_table reposttable(get_self(), reposter.value);
  post_table posttable(get_self(), get_self().value);

  auto postitr = posttable.find(post_uuid);
  if (postitr != posttable.end()) {
    reposttable.emplace(get_self(), [&]( auto& row ) {
      row.reposter = reposter;
      row.post_uuid = post_uuid;
    });
  } else {
    arisen::check(false, "You cannot repost a post that does not exist.");
  };
};

[[arisen::action]]
void unrepost(name reposter,
                      uint64_t post_uuid) {

  repost_table reposttable(get_self(), reposter.value);
  auto repostitr = reposttable.find(post_uuid);
  if (repostitr != reposttable.end()) {
    require_auth(repostitr->reposter);
    reposttable.erase(repostitr);
  } else {
    arisen::check(false, "You cannot unrepost a repost, that does not exist.");
  };

};

[[arisen::action]]
void upvote(name from,
                   name to,
                   arisen::asset amount
                   uint64_t post_uuid) {

  require_auth(from);
  upvote_table upvotetable(get_self(), get_self().value);
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  if (postitr != posttable.end()) {
    // SEND RIX
    action {
      permission_level{get_self(), "active"_n},
      "arisen.token"_n,
      "transfer"_n,
      std::make_tuple(from, to, amount, std::string("You got upvoted!"))
    }.send();
    upvotetable.emplace(get_self(), [&]( auto& row ){
      row.from = from;
      row.to = to;
      row.amount = amount;
      row.post_uuid = post_uuid; 
      row.timestamp = timestamp();
    });
  } else {
    arisen::check(false, "You cannot upvote a post that does not exist.")
  }
};

[[arisen::action]]
void pin(name pinner,
             uint64_t post_uuid) {
  require_auth(pinner);
  pin_table pintable(get_self(), get_self().value);
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  if (postitr != posttable.end()) {
    pintable.emplace(get_self(), [&]( auto& row ){
      row.pinner = pinner;
      row.post_uuid = post_uuid;
      row.timestamp = timestamp();
    });
  } else {
    arisen::check(false, "You cannot pin a post that does not exist.");
  };
};

[[arisen::action]]
void unpin(name pinner, 
                 uint64_t post_uuid) {
  pin_table pintable(get_self(), get_self().value);
  auto pinitr = pintable.find(pinner);

  if (pinitr != pintable.end()) {
    pintable.erase(pinner);
  } else {
    arisen::check(false, "You cannot erase a pin that does not exist.");
  };
};

[[arisen::action]]
void favorite(uint64_t post_uuid,
                    name user) {
  require_auth(user);
  favorite_table favoritetable(get_self(), user.value);
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  
  if (postitr != posttable.end()) {
    favoritetable.emplace(get_self(), [&](auto& row ){
      row.post_uuid = post_uuid;
      row.user = user;
    });
  } else {
    arisen::check(false, "You cannot favorite a post that does not exist.");
  };
};

[[arisen::action]]
void unfavorite(uint64_t post_uuid,
                        name user) {

  favorite_table favoritetable(get_self(), user.value);
  auto favitr = favoritetable.find(post_uuid);
  if (favitr != favoritetable.end()) {
    favoritetable.erase(favitr);
  } else {
    arisen::check(false, "You cannot remove a favorite that does not exist.");
  }
};

[[arisen::action]]
void follow(name follower,
                 name following) {

  require_auth(follower);
  follow_table followtable(get_self(), follower.value);
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(following);
  if (useritr != usertable.end()) {
    followtable.emplace(get_self(), [&]( auto& row ){
      row.follower = follower;
      row.following = following;
    });
  } else {
    arisen::check(false, "You cannot follow a user who does not exist.");
  };
};

[[arisen::action]]
void unfollow(name follower,
                     name following) {
  follow_table followtable(get_self(), follower.value);
  auto followitr = followtable.find(following);
  if (followitr != followtable.end()) {
    require_auth(followitr->follower);
    followtable.erase(following);
  } else {
    arisen::check(false, "You cannot unfollow someone that you are not following.");
  };
};

[[arisen::action]]
void block(name blocker,
                name blocked) {
  require_auth(blocker);
  block_table blocktable(get_self(), blocker.value);
  user_table usertable(get_self(), get_self());
  auto useritr = usertable.find(blocked);

  if (useritr != usertable.end()) {
    usertable.emplace(get_self(), [&]( auto& row ) {
      row.blocker = blocker; 
      row.blocked = blocked;
    });
  } else {
    arisen::check(false, "You cannot block a user that does not exist.");
  };
};

[[arisen::action]]
void unblock(name blocker,
                    name blocked) {
  require_auth(blocker);
  block_table blocktable(get_self(), blocker.value);
  auto blockitr = blocktable.find(blocked);
  if (blockitr != blocktable.end()) {
    require_auth(blockitr->blocker);
    blocktable.erase(blockitr);
  } else {
    arisen::check(false, "You cannot unblock a user that is not blocked.");
  };
};

[[arisen::action]]
void hide(name hider,
               uint64_t post_uuid) {
  require_auth(hider);
  hide_table hidetable(get_self(), hider.value);
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  if (postitr != posttable.end()) {
    hidetable.emplace(get_self(), [&]( auto& row ) {
      row.hider = hider;
      row.post_uuid = post_uuid;
      row.timestamp = timestamp();
    });
  } else {
    arisen::check(false, "You cannot hide a post that does not exist.");
  };
};

[[arisen::action]]
void unhide(name hider,
                  uint64_t post_uuid) {
  hide_table hidetable(get_self(), hider.value);
  auto hideitr = hidetable.find(post_uuid);
  if (hideitr != hidetable.end()) {
    require_auth(hideitr->hider);
    hidetable.erase(hideitr);
  } else {
    arisen::check(false, "You cannot unhide a post that is not hidden.");
  };
};

[[arisen::action]]
void activate(name user) {
  require_auth(user);
  user_table usertable(get_self(), get_self().value);
  status_table statustable(get_self(), get_self().value);
  auto useritr = usertable.find(user);
  if (useritr != usertable.end()) {
    status.emplace(get_self(), [&]( auto& row ){
      row.user = user;
    });
  } else {
    arisen::check(false, "You cannot activate a user that does not exist.");
  };
};

[[arisen::action]]
void deactivate(name user) {
  status_table statustable(get_self(), get_self().value);
  auto statusitr = statustable.find(user);
  if (statusitr != statustable.end()) {
    require_auth(statusitr->user);
    statustable.erase(user);
  } else {
    arisen::check(false, "You cannot deactivate a user that is not yet activated.");
  };
};

[[arisen::action]]
void pimage(name uploader,
                   dwebhash_t drive,
                   std::string filename) {
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(uploader);
  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&]( auto& row ) {
      row.pi_drive = drive;
      row.pfn = filename;
    });
  } else {
    arisen::check(false, "You cannot upload a profile image for a user that does not exist.");
  };
};

[[arisen::action]]
void himage(name uploader,
                   dwebhash_t drive,
                   std::string filename) {
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(uploader);
  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&]( auto& row ){
      row.hi_drive = drive;
      row.hfn = filename;
    });
  } else {
    arisen::check(false, "You cannot upload header image for a user that does not exist.");
  };
};

[[arisen::action]]
void updatedn(name user,
                      std::string display_name) {
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(user);
  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&], ( auto& row ){
      row.display_name = display_name;
    });
  } else {
    arisen::check(false, "You cannot upload the profile display name for a user that does not exist.");
  };
};

[[arisen::action]]
void updatelocation(name user,
                              std::string location){
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(user);
  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&], ( auto& row ){
      row.location = location;
    });
  } else {
    arisen::check(false, "You cannot update the location of a user that does not exist.");
  };
};

[[arisen::action]]
void updateurl(name user,
                      std::string url);

  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(user);

  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&]( auto& row) {
      row.url = url;
    });
  } else {
    arisen::check(false, "You cannot update the profile URL for a user that does not exist.");
  };
};

[[arisen::action]]
void updatebio(name user,
                       std::string bio) {
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(user);
  
  if (useritr != usertable.end()) {
    require_auth(useritr->user);
    usertable.modify(useritr, get_self(), [&]( auto& row {
      row.bio = bio;
    });
  } else {
    arisen::check(false, "You cannot update the profile bio for a user who does not exist.");
  };
};
