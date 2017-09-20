
// JS COULDNT FIND THE ELEMENT WITHOUT window.onload;
// K I L L   J A V A   S C R I P T

var login_wrap;
var login_header;
var register_button;
global.login;
var register;
var create;
var cancel;
window.addEventListener("load", function () {

  // Define var's for transition;
  login_header = document.getElementById("loginstration-header");
  login_wrap = document.getElementById("loginstration-wrap");
  register_button = document.getElementById('loginstration-registration-button');
  login = document.getElementById('loginstration-login');
  register = document.getElementById('loginstration-registration');
  create = document.getElementById("register-button-submit");
  cancel = document.getElementById("register-button-cancel");
  register_button.addEventListener("click" , prepareRegister);
  create.addEventListener("click" , prepareLogin);
  cancel.addEventListener("click" , prepareLogin);
});

function prepareRegister(){
  login_header.classList.remove("displayYes");
  login_wrap.className = "sockSpinTo";
  login.className = "displayNo";
  login_header.className += " " + "displayNo";
  register_button.className = "displayNo"

  login.style.display = "none";
  register.style.display = "flex";
  register.className = "displayYes";
  login_header.classList.remove("displayNo");
  login_header.className += " " + "displayYes";

}

function prepareLogin(){
  login_wrap.className = "sockSpinBack";
  register.classList.remove("displayYes");
  register.className = "displayNo";
  register.style.display = "none";

  login_header.classList.remove("displayNo");
  login_header.className += " " + "displayYes";

  login.style.display = "flex";
  login.classList.remove = "displayNo"
  login.className = "displayYes";
  login_header.className += " " + "displayYes";
  register_button.className = "displayYes"

}
