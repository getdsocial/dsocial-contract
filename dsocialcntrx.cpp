// Create post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::post(name poster,
                                   std::string data,
                                   dwebhash_t drive) {

  require_auth(poster);

  // Table definition 
  post_table posttable(get_self(), get_self().value);
  
  // Validate post inputs
  arisen::check(data.size() <= MAX_POST_SIZE, "Post must be 140 characters or less");
  arisen::check(drive.size() == DRIVE_URL_SIZE, "Drive URL must be 64 hexadecimal characters.");

  // Get Post UUID
  uint64_t uuid = posttable.available_primary_key();

  // Store the post on-chain
  posttable.emplace(get_self(), [&]( auto& p ) {
    p.uuid = uuid;
    p.poster = poster;
    p.data = data;
    p.drive = drive;
    p.timestamp = timestamp();
  });
};

// Modify a post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::modpost(uint64_t uuid,
                                         std::string data,
                                         dwebhash_t drive) {

  // Table Definition
  post_table posttable(get_self(), get_self().value);
  
  // Validate Inputs
  arisen::check(data.size() <= MAX_POST_SIZE, "Post must be 140 characters or less.");
  arisen::check(drive.size() == DRIVE_URL_SIZE, "Drive URL must be 64 hexadecimal characters.");
  
  // Check for Post UUID in the database
  auto iterator = posttable.find(uuid);
  
  // If exists, write modified data at iterator, if modifier is the original creator
  require_auth(iterator->poster);
  if (iterator != posttable.end()) {
    posttable.modify(iterator, get_self(), [&]( auto& row ) {
      row.data = data;
      row.drive = drive;
      row.timestamp = timestamp;
    });
  } else {
    arisen::check(false, "Post does not exist for the given UUID.");
  }
};

// Remove a post from dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unpost(uint64_t uuid) {

  // Table Definition
  post_table posttable(get_self(), get_self().value);

  // Find Post UUID in database
  auto iterator = posttable.find(uuid);

  // Check that hte post actually exists
  arisen::check(iterator != posttable.end(), "Post doesn't exist for the given UUID.");

  // Make sure the deleter is the original creator of the post
  require_auth(iterator->poster);

  // Delete post
  posttable.erase(iterator);
};

// Create a comment, under a given post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::comment(name commenter,
                                         uint64_t post_uuid,
                                         std::string data,
                                         dwebhash_t drive) {
  // Comment signer
  require_auth(commenter);

  // Table Definition
  comment_table commenttable(get_self(), get_self().value);
  
  // Validate Inputs
  arisen::check(data.size() <= MAX_COMMENT_SIZE, "Comment must be 140 characters or less.");
  arisen::check(drive.size() == DRIVE_URL_SIZE || null, "Drive URL must be 64 hexadecimal characters.");

  // Get Comment UUID
  uint64_t comment_uuid = commenttable.available_primary_key();
  
  // Check Posts Table For Post UUID
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  
  // If post is found, store comment, if not, issue error
  if (postitr != posttable.end()) {
    commenttable.emplace(get_self(), [&]( auto& c ) {
      c.comment_uuid = comment_uuid;
      c.commenter = commenter;
      c.post_uuid = post_uuid;
      c.data = data;
      c.drive = drive;
      c.timestamp = timestamp();
    });
  } else {
    arisen::check(false, "The given post UUID you're attempting to comment on does not exist.");
  }
};

// Modify an already existent comment on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::modcomment(uint64_t comment_uuid,
                                                 std::string data,
                                                 dwebhash_t drive) {
  
  // Table Definition
  comment_table commenttable(get_self(), get_self().value);

  // Validate comment inputs
  arisen::check(data.size() <= MAX_COMMENT_SIZE, "Comment must be 140 characters or less.");
  arisen::check(drive.size() == DRIVE_URL_SIZE, "Drive URL must be 64 hexadecimal characters.");

  // Check for comment UUID in DB
  auto commentitr = commenttable.find(comment_uuid);

  // Define signer and make sure they're the original creator of the comment being modified
  require_auth(commentitr->commenter);

  if(commentitr != commenttable.end()) {
    commenttable.modify(commentitr, get_self(), [&]( auto& row ) {
      row.data = data;
      row.drive = drive;
      row.timestamp = timestamp();
    });
  } else {
    arisen::check(false, "Comment UUID does not exist.");
  } 
};

// Delete a comment on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::uncomment(uint64_t comment_uuid) {
  // Table Definition
  comment_table commenttable(get_self(), get_self().value);

  // Find Comment UUID
  auto iterator = commenttable.find(comment_uuid);

  // Check that the comment exists
  arisen::check(iterator != commenttable.end(), "Comment doesn't exist for the given UUID.");

  // Make sure the deleter is the comment's original author
  require_auth(iterator->commenter);

  // Delete comment
  commenttable.erase(iterator);
};

// Love a post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::love(uint64_t uuid_pointer,
                                   bool is_post,
                                   name lover) {
  
  // Define Signer
  require_auth(lover);
  
  // Table Definition
  love_table lovetable(get_self(), uuid_pointer);

  // Check for Post UUID
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(uuid_pointer);

  // Make sure this post hasn't been loved before by this user
  auto loveitr = lovetable.find(lover);

  // If UUID pointer exists and it hasn't been loved by this user before, store some love on-chain for this post
  if (postitr != posttable.end() && loveitr == lovetable.end()) {
    lovetable.emplace(get_self(), [&]( auto& l ) {
      l.lover = lover;
      l.uuid_pointer = uuid_pointer;
      l.is_post = is_post;
    });
  } else if (postitr == posttable.end()) { 
    arisen::check(false, "You cannot love a post that does not exist.");
  } else if (loveitr != lovetable.end()) {
    arisen::check(false, "You have already loved this post
  }
};

// Unlove a post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unlove(uint64_t lover,
                                      uint64_t uuid_pointer) {
 
  // Table Definition
  love_table lovetable(get_self(), uuid_pointer);

  //Create love iterator and check to see that the love was actually given.
  auto loveitr = lovetable.get(lover, "Can't find the love.");

  // Make sure this is the actual lover
  require_auth(loveitr->lover);

  // Remove the love
  lovetable.erase(loveitr);
};

// Repost a post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::repost(name reposter,
                                      uint64_t post_uuid) {
  require_auth(reposter);
  
  // Repost table definition. Make sure we scope the table to the reposter, so that they can
  // only repost a post once.

  repost_table reposttable(get_self(), reposter.value);

  // Check for post in DB
  post_table posttable(get_self(), get_self().value);
  auto postitr = posttable.find(post_uuid);
  
  // Make sure that user has not reposted this post before
  auto repostitr = reposttable.find(post_uuid);
  arisen::check(repostitr == reposttable.end(), "You cannot repost a post you have already reposted.");

  // Add repost to the on-chain DB
  reposttable.emplace(get_self(), [&]( auto& r ) {
    r.reposter = reposter;
    r.post_uuid = post_uuid;
  });
};

// Remove a repost from dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unrepost(uint64_t post_uuid,
                                          name reposter) {
  // Table Definition
  repost_table reposttable(get_self(), reposter.value);

  // Check for the repost in the DB
  auto iterator = reposttable.find(post_uuid);
  arisen::check(iterator != reposttable.end(), "You cannot delete a repost that doesn't exist.");

  // Ensure that the deleter is the reposter
  require_auth(iterator->reposter);
  reposttable.erase(iterator);
};

// Favorite a post on dSocial
[[arisen::action]]
void dsocialcntrx:favorite(uint64_t post_uuid,
                                       name user) {

  // Table Definition
  favorite_table favoritetable(get_self(), get_self().value);

  // Create iterator to see if post has already been favorited
  auto favitr = favoritetable.find(post_uuid);
  arisen::check(favitr == favoritetable.end(), "You have already favorited this post.");

  // Get favorite UUID
  uint64_t uuid = favoritetable.available_primary_key();

  // Get signer
  require_auth(user)

  // Store favorite on-chain
  favoritetable.emplace(get_self(), [&]( auto& f ) {
    f.uuid = uuid;
    f.post_uuid = post_uuid;
    f.user = user;
  });
};

// Unfavorite a post on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unfavorite(uint64_t uuid) {
  // Table Definition
  favorite_table favoritetable(get_self(), get_self().value);

  // Check for favorite
  auto favitr = favoritetable.find(uuid);
  arisen::check(favitr != favoritetable.end(), "Was unable to find a favorite.");
  
  // Unfavorite
  require_auth(favitr->user);
  favoritetable.erase(uuid);
};

// Pin a post to the top of a profile
//@abi action
[[arisen::action]]
void dsocialcntrx::pin(name pinner, 
                                 uint64_t content_uuid) {
  // Table Definition
  pin_table pintable(get_self(), get_self().value);

  // Create pintable iterator 
  auto pinitr = pintable.find(pinner);
 
  // The above allows us to find the pinner's current pin. If the pin exists, it will modify the current pin.
  // If no pin exists, it will create a new one.

  if (pinitr == pintable.end()) {
    require_auth(pinner);
    pintable.emplace(get_self(), [&]( auto& p ) {
      p.pinner = pinner;
      p.content_uuid = content_uuid;
      p.timestamp = timestamp();
    });
  } else {
    require_auth(pinitr->pinner);
    pintable.modify(pinitr, get_self(), [&]( auto& row ) {
      row.content_uuid = content_uuid;
      row.timestamp = timestamp();
    });
  }
};

// Remove an active profile pin on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unpin(name pinner) {
  // Table Definition
  pin_table pintable(get_self(), get_self().value);

  // Create pin iterator and check for an active pin
  auto pinitr = pintable.find(pinner);
  arisen::check(pinitr != pintable.end(), "You have no pin to remove");

  require_auth(pinitr->pinner);
  pintable.erase(pinitr);
};

// Follow a user on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::follow(name follower,
                                     name following) {

  // Check for to-be followed user in the active user table
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(following);
  arisen::check(useritr != usertable.end(), "The user you're attempting to follow, is not an active dSocial user.");

  // Check to see if the executor is already following this user (Table scoped to follower)
  follow_table followtable(get_self(), follower.value);
  auto followitr = followtable.find(following)
  arisen::check(followitr == followtable.end(), "You are already following this user.");

  // Sign for the follow
  require_auth(follower)

  // Follow user
  followtable.emplace(get_self(), [&]( auto& f ) {
    f.following = following;
    f.follower = follower;
  });
};

// Unfollow a user on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unfollow(name follower,
                                         name following) {

  // Check to see if the executor is following this user
  follow_table followtable(get_self(), follower.value);
  auto followitr = followtable.find(following);
  arisen::check(followitr != followtable.end(), "You are not following this user.");

  // Make sure this is actually one of their actual followers
  require_auth(followitr->follower);
  followitr.erase(following);
};

// Block a user on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::block(name blocker,
                                    name blocked) {

  // Table definition = scoped to blocker
  block_table blocktable(get_self(), blocker.value);

  // Check to see if user that is being blocked, is a real user
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.find(blocked);
  arisen::check(useritr != usertable.end(), "The user you're attempting to block, is not an active dSocial user.");
  
  // Check to see if the user being blocked is already blocked
  auto blockitr = blocktable.find(blocked);
  arisen::check(blockitr == blocktable.end(), "You have already blocked this user.");

  // Get blocker signature
  require_auth(blocker);
  
  // Block user
  blocktable.emplace(get_self(), [&]( auto& b ) {
    b.blocked = blocked;
    b.blocker = blocker;
  });
};

// Unblock a user on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unblock(name blocker,
                                        name blocked) {
  // Table Definition
  block_table blocktable(get_self(), blocker.value);

  // Check to see if user being unblocked, is not actually blocked at the time
  auto blockitr = blocktable.find(blocked);
  arisen::check(blockitr != blocktable.end(), "The user you're attempting to unblock, is not blocked");
  
  // Make sure it's the original blocker
  require_auth(blockitr->blocker);

  blocktable.erase(blockitr);
};

// Hide a post or comment on dSocial 
//@abi action
[[arisen::action]]
void dsocialcntrx::hide(name hider,
                                   uint64_t content_uuid) {
  // Check to see if the content exists
  post_table posttable(get_self(), get_self().value);
  comment_table commenttable(get_self(), get_self().value);
  auto postitr = posttable.find(content_uuid);
  auto commentitr = commenttable.find(content_uuid);
  arisen::check(postitr != posttable.end() && commentitr != commenttable.end(), "Cannot hide content that doesn't exist.");

  // Table definition
  hide_table hidetable(get_self(), get_self().value);

  // Check to see if content is already hidden by hider
  auto hideitr = hidetable.find(content_uuid);
  arisen::check(hideitr == hidetable.end(), "You have already hidden this content.");

  // Get signature of hider
  require_auth(hider);

  // Store on-chain
  hidetable.emplace(get_self(), [&]( auto& h ) {
    h.hider = hider;
    h.content_uuid = content_uuid;
    h.timestamp = timestamp();
  });
};

// Unhide a post or comment on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::unhide(uint64_t content_uuid) {
  // Check to see if the content is hidden
  hide_table hidetable(get_self(), get_self().value);
  auto hideitr = hidetable.find(content_uuid);
  arisen::check(hideitr != hidetable.end(), "Post or comment must be hidden first.");

  // Make sure unhider is the original hider
  require_auth(hideitr->hider);
  // Unhide
  hidetable.erase(hideitr);
};

// Save Profile Data
//@abi action
[[arisen::action]]
void dsocialcntrx::pdata(name user,
                                     name display_name,
                                     std::string url, 
                                     name bio,
                                     name location) {
  require_auth(auth);
  
  // Table Definition
  user_table usertable(get_self(), get_self().value);

  // Set iterator
  auto useritr = usertable.find(user);

  // Check if user already exists and if so, modify record
  if (useritr == usertable.end()) {
    usertable.emplace(get_self(), [&]( auto& u ) {
      u.user = user;
      u.display_name = display_name;
      u.url = url;
      u.bio = bio;
      u.location = location;
    });
  } else {
    usertable.modify(useritr, get_self(), [&]( auto& row ) {
      row.user = user;
      row.display_name = display_name;
      row.url = url;
      row.bio = bio;
      row.location = location;
    });
  }
};

// Activate a user on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::activate(name user) {
  // Check to see if user is already activated
  status_table statustable(get_self(), get_self().value);
  auto statusitr = statustable.find(user);
  arisen::check(statusitr == statustable.end(), "Account already activated.");

  require_auth(user);
  statustable.emplace(get_self(), [&]( auto& a ) {
    a.user = user;
  });
};

// Deactivate a user on dSocial
[[arisen::action]]
void dsocialcntrx::deactivate(name user) {
  // Check to see if user is activated
  status_table statustable(get_self(), get_self().value);
  auto statusitr = statustable.find(user);
  arisen::check(status != statustable.end(), "Account is already deactivated.");

  // One the user can deactivate themselves
  require_auth(statusitr->user);

  statustable.erase(user);
};

// Add of update a dSocial profile image
//@abi action
[[arisen::action]]
void dsocialcntrx::pimage(name uploader,
                                       dwebhash_t drive,
                                       std::string filename) {
  // Table definition
  pimage_table pimagetable(get_self(), get_self().value);
  
  // Create iterator
  auto pimitr = pimagetable.find(uploader);
  // If profile image exists, modify it, if not, add a new one
  if (pimitr == pimagetable.end()) {
    require_auth(uploader);
    pimagetable.emplace(get_self(), [&]( auto& p ) {
      p.uploader = uploader;
      p.drive = drive;
      p.filename = filename;
    });
  } else {
    require_auth(pimitr->uploader);
    pimagetable.modify(pimitr, get_self(), [&]( auto& row ) {
      row.drive = drive;
      row.filename = filename;
    });
  }
};

// Add or update a dSocial profile header image
//@abi action
[[arisen::action]]
void dsocialcntrx::himage(name uploader,
                                       dwebhash_t drive,
                                       std::string filename) {

  // Table Definition
  himage_table himagetable(get_self(), get_self().value);
  
  // Create iterator
  auto himitr = himagetable.find(uploader);
  
  // If header image exists, modify it, if not, add a new one
  if (himitr == himagetable.end()) {
    require_auth(uploader);
    himagetable.emplace(get_self(), [&]( auto& h ) {
      h.uploader = uploader;
      h.drive = drive;
      h.filename = filename;
    });
  } else {
    require_auth(himitr->uploader);
    himagetable.modify(himitr, get_self(), [&]( auto& row ) {
      row.drive = drive;
      row.filename = filename;
    });
  }
};

// Update profile bio on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::updatebio(name user,
                                           name bio) {

  // Check for the existence of the user
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.get(user, "You cannot update the bio for a user who is not active.");
  
  // Validate bio size
  arisen::check(bio <= MAX_PROFILE_BIO_SIZE, "Your profile bio can only be 140 characters.");

  // Make sure the modifier is the original creator
  require_auth(useritr.user);

  // Update bio
  usertable.modify(useritr, get_self(), [&]( auto& b ) {
    b.bio = bio;
  });
};

// Update profile location on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::updatelocation(name user,
                                                  name location) {
  // Check for the existence of the user
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.get(user, "You cannot update the location for a user who is not active.");

  // Validate location data size
  arisen::check(location <= MAX_PROFILE_LOCATION_SIZE, "Your profile location can only be 64 characters");
  // Make sure the modifier is the original creator
  require_auth(useritr.user);
  
  // Update location
  usertable.modify(useritr, get_self(), [&]( auto& l ) {
    l.location = location;
  });
};

// Update profile display name on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::updatename(name user,
                                               name display_name) {

  // Check for the existence of the user
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.get(user, "You cannot update the display name for a user who is not active.");

  // Validate the size of the display name data
  arisen::check(display_name <= MAX_PROFILE_DISPLAY_NAME_SIZE, "Your profile display name can only be 32 characters.");

  // Make sure the modifier is the original creator
  require_auth(useritr.user);

  // Update display name
  usertable.modify(useritr, get_self(), [&]( auto& d ) {
    d.display_name = display_name;
  });
};

// Update profile URL on dSocial
//@abi action
[[arisen::action]]
void dsocialcntrx::updateurl(name user,
                                          std::string url) {
  
  // Check for the existence of the user
  user_table usertable(get_self(), get_self().value);
  auto useritr = usertable.get(user, "You cannot update the profile URL for a user who is not active.");

  // Validate profile url data
  arisen::check(url <= MAX_PROFILE_URL_SIZE, "Your profile url can only be 64 characters.");

  // Make sure the modifier is the original creator
  usertable.modify(useritr, get_self(), [&]( auto& u ) {
    u.url = url;
  });
};

// Upvote a post or comment on dSocial with RIX
//@abi action
[[arisen::action]]
void dsocial::upvote(uint64_t content_uuid,
                               name from,
                               name to,
                               arisen::asset payment_total) {

  // Get upvoter's signature
  require_auth(from);

  // Table Definition
  upvote_table upvotetable(get_self(), content_uuid);

  // Make sure the content exists
  post_table posttable(get_self(), get_self().value);
  comment_table commenttable(get_self(), get_self().value);
  auto postitr = posttable.get(content_uuid, "The post you are attempting to upvote does not exist");
  auto commentitr = commenttable.get(content_uuid, "The comment you are attempting to upvote does not exist");
  arisen::check(postitr != posttable.end() || commentitr != commenttable.end(), "The content you are upvoting does not exist");

  // Make sure the payment_total is valid
  arisen::check(payment_total.is_valid(), "The payment total is not valid.");
  
  // Get UUID for payment
  uint64_t uuid = upvotetable.available_primary_key();

  // Send RIX 
  action {
    permission_level{get_self(), "active"_n},
    "arisen.token"_n,
    "transfer"_n,
    std::make_tuple(from, to, payment_total, std::string("You got upvoted on dSocial!"))
  }.send();
  
  // Save record of the upvote on-chain
  upvotetable.emplace(get_self(), [&]( auto& u ) {
    u.uuid = uuid;
    u.from = from;
    u.to = to;
    u.payment_total = payment_total;
    u.timestamp = timestamp();
  });    
};

// TODO: Add upvoteremote action for handing upvotes with Bitcoin, Ethereum, EOS and TRON (off-chain currencies)
