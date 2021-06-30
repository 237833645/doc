/**
 * Created by Sequel on 2018/5/31.
 */
$(function () {
    /*layui.use(['layer'],function(){

    });*/
    $('.layui-input,.layui-textarea').each(function () {
        $(this).placeholder();
        $(this).focus(function () {
            $(this).removeClass("bdc-f6");
        })
    });
    blogObj.mainMinHight();
    $(window).resize(function () {
        blogObj.mainMinHight();
    });
    blogObj.rightFixde();
    $(window).on("resize", blogObj.rightFixde);
});


$.extend({
    /**
     * ajax扩展，带登录过期验证
     * @param opt
     * @private
     */
    _ajax: function (opt) {
        var success = opt.success;
        opt.data = opt.data || {};
        opt.data['_t'] = (new Date()).getTime();
        opt.success = function (data, status, xhr) {
            if (data.code == 100) {
                //调整到登录页面
                document.location.href = blog.udomain + '/login?url=' + encodeURIComponent(document.location.href);
                return;
            }
            success(data, status, xhr);
        }
        $.ajax(opt);
    },
    htmlDeCode: function (str) {//html转义成字符实体   '<' 转成 '&lt;'
        //return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&apos;");
        return str.replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&apos;");
    },
    getLen: function (str) {
        var len = str.length;
        var count = 0;
        for (var i = 0; i < len; i++) {
            count = count + ((str.charCodeAt(i) > 126 || str.charCodeAt(i) < 27) ? 2 : 1);
        }
        return count;
    }
});

/*共用库*/
(function () {
    var blog = {};
    var u = null;
    blog.udomain = '//account.itpub.net';
    blog.img_domain = '//img.blog.itpub.net';
    blog.cookie = {};
    blog.cookie.set = function (a, b, d) {
        var g;
        d.s && (g = new Date,
            g.setTime(g.getTime() + d.s));
        document.cookie = a + "=" + b + (d.domain ? "; domain=" + d.domain : "") + (d.path ? "; path=" + d.path : "") + (g ? "; expires=" + g.toGMTString() : "") + (d.wb ? "; secure" : "")
    }
    ;
    blog.cookie.get = function (a) {
        return (a = RegExp("(^| )" + a + "=([^;]*)(;|$)").exec(document.cookie)) ? a[2] : u
    };

    blog.user = {
        /*获取用户信息*/
        get: function () {
            //return null;
            var u = blog.cookie.get('blog_user');
            if (!u) return null;
            u = decodeURIComponent(u).split('|');
            try {
                return {
                    'userid': u[0],
                    'role': u[1],
                    'username': u[2],
                    'headimg': decodeURIComponent(u[3]),
                }
            }
            catch (e) {
                return null;
            }
        },
        /*清空用户信息*/
        clear: function () {
            d = {s: -1000000000, path: '/'};
            blog.cookie.set('blog_user', '', d);
        },

        getHeadImg: function (url) {
            return url + (url.indexOf('?') > 0 ? '&' : '?') + (new Date()).getTime();
        }
    };

    blog.url = {
        /*获取url参数*/
        queryString: function (url, name) {
            var d = url.match(RegExp("(^|&|\\?|#)(" + name + ")=([^&#]*)(&|$|#)", ""));
            return d ? decodeURIComponent(d[3]) : u;
        }
    };

    blog.loginout = function (e) {
        var url = blog.udomain + '/login/out';
        var ref = typeof __il_ == 'undefined' ? document.location.href : 'http://blog.itpub.net/';
        url = url + '?url=' + encodeURIComponent(ref);
        $(e.target).attr('href', url);
    };
    blog.login = function (e) {
        var url = blog.udomain + '/login';
        var ref = document.location.href;
        url = url + '?url=' + encodeURIComponent(ref);
        $(e.target).attr('href', url);
    }

    window.blog = blog;
})();

/*渲染登录信息*/
$(function () {
    /*为main添加最小高度*/
    $(".header").siblings(".main").css({
        "minHeight": $(window).height() - $(".header").height() - $(".footer").height() + "px"
    })
    var islogin = false;
    var setuserinfo = function () {
        var uinfo = $('#u_info');
        var html = [];
        var user = blog.user.get();
        if (!user) {
            islogin = false;
            html.push('<a class="blog-login">登录</a>');
            html.push('<a href="' + blog.udomain + '/register" target="_blank" class="blog-register">注册</a>');
            uinfo.removeClass('icon-down');
        }
        else {
            islogin = true;
            html.push('<a href="/myarticle/">');
            html.push('<img class="head-img" src="' + (blog.user.getHeadImg(user.headimg)) + '" alt="">');
            html.push('</a>');
            html.push('<div class="dropdown-menu">');
            html.push('<a href="/settings/" class="icon-account">个人中心</a>');
            html.push('<a href="/mynotice/" class="icon-news">消息中心</a>');
            html.push('<a href="' + blog.udomain + '/ucenter/user/index" class="icon-set" >账户设置</a>');
            html.push('<a href="' + blog.udomain + '/login/out" class="icon-exit" id="user_exit">退出登录</a>');
            html.push('</div>');
            $('.icon-news').show();
            uinfo.addClass('icon-down');
        }
        uinfo.html(html.join(''));
        if (!islogin) $('.blog-login').click(blog.login);
        if (islogin) $('#user_exit').click(blog.loginout);
    }
    setuserinfo();
    $._ajax({
        url: '/user/info/',
        type: 'get',
        dataType: 'json',
        success: function (data) {
            var login = data.data.login;
            login != islogin && setuserinfo();
            if (!login) {
                blog.user.clear();
            } else {
                data.data.message && $('.icon-news').addClass('news-unread');
            }
            typeof userHook !== 'undefined' && typeof userHook === 'function' && userHook();
        }
    });
});


var blogObj = {
    alert: function (string, time, url) {
        var time = time ? time : 1500;
        if ($("#blog_alert").length) {
            clearTimeout(timer)
            $("#blog_alert").show(160).text(string);
        } else {

            $('body').append('<div class="blog-alert" id="blog_alert">' + string + '</div>');
        }
        $("#blog_alert").show(160);
        setTimeout(function () {
            $("#blog_alert").hide(160);
            setTimeout(function () {
                $("#blog_alert").remove()
            }, 160);
            if (url) {
                window.location.href = url;
            }
        }, time);

    },
    message: function (msg) {
        $('body').append('<div class="blog-msg" style=" top: ' + ($(window).height() - 120) / 2 + 'px; left: ' + ($(window).width() - 360) / 2 + 'px">' +
            '<div class="icon-close" onclick=$(this).parent().remove()></div>' +
            '<p class="title" style="padding-top:40px;">'+msg+'</p>' +
            '</div>');

    },
    /*弹框*/
    msg: function (href, msg) {
        $('body').append('<div class="blog-msg" style=" top: ' + ($(window).height() - 120) / 2 + 'px; left: ' + ($(window).width() - 360) / 2 + 'px">' +
            '<div class="icon-close" onclick=$(this).parent().remove()></div>' +
            '<p class="title">' + (msg == null ? "发布成功" : msg) + '</p>' +
            '<p class="content"><a href="' + href + '">即将跳转查看</a> &nbsp;&nbsp;&nbsp;<span class="count">3s</span></p>' +
            '</div>');
        blogObj.countDown(3, 0, function (count, minTime) {
            if (count < minTime) {
                window.location.href = href;
                return;
            }
            ;
            $(".blog-msg .count").text(count + "s");

        });

    },

    banner: function (time) {
        var banner = $("#banner");
        var ulis = banner.children("ul").children("li");
        var count = ulis.length;
        var num = 0;
        var timer = null;
        timer = setInterval(change, time);

        var uls = $("#banner>ul").clone();
        uls.css("width",120);
        $(".small-list").append(uls);
        var cLi = $(".small-list ul li").eq(0).addClass("current").end();
        $(".small-list ul").css("width",cLi.outerWidth(true)*count);
        cLi.on("click",function () {
            num = $(this).index()+1;
            change("left");
            return false;
        });


        /*鼠标移上离开停止开启定时器*/
        banner.on("mouseover", function () {
            clearInterval(timer);
        }).on("mouseout", function () {
            clearInterval(timer);
            timer = setInterval(change, time);
        })
        $(".bt-change .right").on("click", function () {
            change();
        })
        $(".bt-change .left").on("click", function () {
            change("left");
        })

        function change(dir) {
            if(dir && dir == "left"){
                num--;
                num = num < 0 ? (count - 1) : num;
            }else {
                num++;
                num = num > (count - 1) ? 0 : num;
            }
            ulis.eq(num).stop().fadeIn().siblings().stop().fadeOut();
            // olis.eq(num).addClass("current").siblings().removeClass("current");
            cLi.eq(num).addClass("current").siblings().removeClass("current");
            var slideIndex = 1;
            slideIndex = num == 0?slideIndex:num-1;
            if(num == 0){
                slideIndex = 0;
            }
            if(num == count-1){
                slideIndex = slideIndex-1;
            }
            cLi.parent().css("left",-slideIndex*cLi.outerWidth(true));
        }
    },

    /*可选择的提示框*/
    confirm: function (string, func) {
        $('body').append('' +
            '<div class="blog-confirm">' +
            '<div class="confirm">' +
            '<div class="close icon-close" onclick=$(this).parents(".blog-confirm").remove()></div>' +
            '<p>' + string + '</p>' +
            '<div class="layui-btn-container">' +
            '<button class="layui-btn mr50 ml20" id="confirm_sure">确定</button>' +
            '<button class="layui-btn layui-btn-primary btn-primary-f2" onclick=$(this).parents(".blog-confirm").remove()>取消' +
            '</button>' +
            '</div>' +
            '</div>' +
            '</div>');
        $("#confirm_sure").on("click", function () {
            $(".blog-confirm").remove();
            func($(this));
        })
    },

    /*删除草稿、博文*/
    delDraft: function (ele,type) {
        var url = '', prompt = '';
        var eleP = ele.parent().parent();
        var blogid = ele.attr('mid');
        var data = {
            id: blogid,
            type:type,
            _token: $("input[name='_token']").val()
        };
        console.log(data)
        if (eleP.hasClass("draft-item")) { //草稿
            url = '/blog/deldraft/';
            prompt = '确定删除此篇草稿吗？';
        } else if ((eleP.hasClass("list-item") && !eleP.hasClass("favour-item")) || ele.parent().hasClass("superman")) { //博文列表 || 管理员删除
            url = '/delarticle/';
            prompt = '确定删除此篇文章吗？';
        } else if (eleP.hasClass("classify-item")) { //我的分类
            url = '/catelist/del/';
            prompt = '确定删除此分类吗？';
        } else if (eleP.hasClass("news-item")) { //我的消息
            url = '/mynotice/del/';
            prompt = '确定删除此消息吗？';
        } else if (ele.parents(".comment-item").length) { //评论管理
            eleP = ele.parents(".comment-item");
            url = '/comment/del/';
            prompt = '确定删除此条评论吗？';
        } else if (eleP.hasClass("favour-item")) { //我的收藏
            url = '/delfavour/';
            prompt = '确定删除此篇收藏吗？';
        }
        ;
        var func = function (element) {
            $._ajax({
                url: url,
                type: 'post',
                dataType: 'json',
                data: data,
                success: function (data) {
                    if (data.code == 200) {
                        element.parents(".blog-confirm").remove()
                        if (ele.parent().hasClass("superman")) {
                            blogObj.alert('删除成功', 1500, '/');
                        } else {
                            eleP.remove();
                            blogObj.alert('删除成功');
                        }
                    } else {
                        blogObj.alert('删除失败');
                    }
                }
            })
        }
        blogObj.confirm(prompt, func);

    },

    /*删除博文*/

    /*时间倒计时*/
    countDown: function (count, minTime, func) {
        var timer = setInterval(function () {
            count--;
            if (count < minTime) {
                clearInterval(timer);
            }
            func(count, minTime);
        }, 1000)
    },


    /*动画添加高度*/
    addHeight: function (ele, height, time) {
        var time = time ? time : 300;
        ele.animate({
            "height": height
        }, time)
    },


    /*获取当前时间*/
    getTime: function () {
        var myDate = new Date();
        var year = myDate.getFullYear();
        var month = (myDate.getMonth() + 1) < 10 ? "0" + (myDate.getMonth() + 1) : (myDate.getMonth() + 1);
        var date = myDate.getDate() < 10 ? "0" + myDate.getDate() : myDate.getDate();
        var h = myDate.getHours() < 10 ? "0" + myDate.getHours() : myDate.getHours();
        var m = myDate.getMinutes() < 10 ? "0" + myDate.getMinutes() : myDate.getMinutes();
        var s = myDate.getSeconds() < 10 ? "0" + myDate.getSeconds() : myDate.getSeconds();
        return year + "-" + month + "-" + date + " " + h + ":" + m + ":" + s;
    },

    /*blog-main模块最小高度*/
    mainMinHight: function () {
        // $(".blog-main").css({
        //     "min-height": $(window).height() - $(".blog-header").outerHeight() - $(".blog-footer").outerHeight()
        // });
        // if ($(".side-nav").length) {
        //     var infoH = $(".author-info").length ? $(".author-info").outerHeight(true) : 0;
        //     $(".side-nav").css({
        //         "min-height": $(".blog-main").height() - 20 - infoH
        //     });
        // }
    },

    /*获取字符串长度*/
    GetLength: function (str) {
        var realLength = 0, len = str.length, charCode = -1;
        for (var i = 0; i < len; i++) {
            charCode = str.charCodeAt(i);
            if (charCode >= 0 && charCode <= 128)
                realLength += 1;
            else
                realLength += 2;
        }
        return realLength;
    },

    /*列表页加载更多*/
    listLoadMore: function (ele, url, count) {
        var url = url ? url : '/datas/list.json';
        var count = count ? count : 15;
        var page = parseInt(ele.attr("data-page"));
        var flag = ele.attr("data-flag");
        var data = {
            page: page
        }
        if (flag != "false") {
            $.ajax({
                url: url,
                type: 'get',
                dataType: 'json',
                data: data,
                success: function (data) {
                    if (data.code == 200) {

                        var data = data.data;
                        var total = data.total;
                        var items = data.items;
                        var list=new Array();
                        for(var i=0;i<items.length;i++)
                        {
                            if(toplist.indexOf(items[i].id)<0)
                            {
                                list.push(items[i]);
                            }
                        }
                        list=items;
                        var obj = {
                            items: list
                        };
                        console.log(items);
                        var resultStr = template('blog_template', obj);
                        $("#list").append(resultStr);

                        if (Math.ceil(total / count) <= page) {
                            ele.attr("data-flag", false);
                            $("#list").siblings(".load-more").text("没有更多了").addClass("colorbb");
                        }
                        ele.attr("data-page", (page + 1));
                    }
                }
            })
        }
    },

    regJudge: {
        "phone": /(^[\d]{1,4} [\d]{7,11}$)|(^1[\d]{10}$)/ //手机号
        , "tel_email": /^((1(\d{10}))|(([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+((\.[a-zA-Z0-9_-]{2,3}){1,2})))$/ //电话或邮箱
        , "code": /^(\d{4})$///验证码
        , "QQ": /^(\d{6,10})$///QQ
        , "pwd": /^(?![0-9]+$)(?![a-zA-Z]+$)[0-9a-zA-Z]{6,16}$/ //密码
        , "email": /^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+((\.[a-zA-Z0-9_-]{2,3}){1,2})$/ //邮箱
        , "all": /./
        //,"applyname": /[a-z]+[0-9a-z_]*(\.[a-z]+[0-9a-z_]*)*/ //应用包名
        , "applyname": /^([a-z_][a-z_0-9]{0,1})+((\.[a-z0-9_]{1,}){1,})$/ //应用包名
        , "route": /^([a-z_][a-z_0-9]{0,1})+((\.[a-z0-9_]{1,}){1,})+(.)*$/ //绝对路径
        , "url": /(http|ftp|https):\/\/[\w\-_]+(\.[\w\-_]+)+([\w\-\.,@?^=%&amp;:/~\+#]*[\w\-\@?^=%&amp;/~\+#])?/ //url
        , "fullname": /^[\u4E00-\u9FA5]{2,4}$/ //姓名
        , "companyname": /^[\w\W]{1,80}$/ //公司名称
        ,"money" : /(^[1-9]{1}[0-9]*$)|(^[0-9]*\.[0-9]{0,2}$)/ //提现金额
        ,"number":/(^[1-9]{1}[0-9]*$)|(^[0-9]$)/ //整型
    },

    prompts: {
        "name": "姓名格式不正确"
        , "title": '标题不能为空' //手机号
        , "tel_email": '手机号或邮箱格式不正确' //手机号
        , "articleType": ['请选择文章类型']//验证码
        , "curpwd": "当前密码输入错误"//验证码
        , "confirmPwd": "两次密码输入不一致" //确认密码
        , "code": "验证码输入错误" //密码
        , "email": "请输入正确的邮箱地址" //密码
        , "applyname": "包名由数字、小写字母、下划线组成，用点号隔开，不能以数字开头。如：com.example.app.base"
        , "fullname": "请输入真实姓名"
        , "fullname": "请输入真实姓名"
        , "route": "绝对路径格式不正确"
        , "format": "格式不正确"
        , "companyname": "请输入公司名称" //公司名称
        , "QQ": "QQ号格式错误" //公司名称
        ,"money":"请输入提现金额\",\"提现金额为大于0的整数或者保留两位数的小数"

    },

    /*验证表单*/
    validatemobile: function (mobile, value, myreg) {
        var mobile_wrap = mobile;
        var mobile = mobile.val().trim();
        var mobileLength = blogObj.GetLength(mobile);
        var promptEle = mobile_wrap.parent().siblings(".icon-prompt")
        /*提示框的样式开始*/
        if (mobileLength == 0) {
            if (mobile_wrap.attr("id") == "link") {
                return true;
            }
            promptEle.show().text("不能为空");
            return false;
        }
        if (!myreg.test(mobile)) {
            promptEle.show().text(value);
            return false;
        } else {
            if (mobileLength > mobile_wrap.attr("maxlength")) {
                promptEle.show().text(value);
                return false;
            }
            promptEle.hide();
            return true;
        }
        /*提示框的样式结束*/
    },


};
/*右侧固定*/

blogObj.rightFixde = function () {
    if($(".right-fixed").length>0) {
        var rightFixedEle = $(".right-fixed").eq($(".right-fixed").length - 1);
        var fixedH = $(".blog-footer").height() + rightFixedEle.outerHeight() + 44
        var fixedTop = $(window).height() > fixedH ? 0 : $(window).height() - fixedH;
        var rightFixedElePH = rightFixedEle.parent().innerHeight();
        //var top = rightFixedElePH+rightFixedEle.parent().offset().top-rightFixedEle.outerHeight(true) - fixedTop;
        /*固定元素的上一个div*/
        var divL = rightFixedEle.parent().children("div").length - 1;
        var rightFixedElePrev, top;
        if (divL) {
            rightFixedElePrev = rightFixedEle.parent().children("div").eq(divL - 1);
            top = rightFixedElePrev.offset().top + rightFixedElePrev.outerHeight(true) - fixedTop;
        } else {
            rightFixedElePrev = rightFixedEle.parent();
            top = rightFixedElePrev.offset().top - fixedTop;
        }
        var left = rightFixedEle.parent().offset().left;
        //console.log(rightFixedEle.parent().attr("class"));
        //console.log(rightFixedEle.parent().offset().left);
        fixed();
        $(window).on("scroll", function () {
            fixed();
        });

        function fixed() {
            var sTop = $(document).scrollTop();
            /*返回顶部按钮*/
            if (sTop > $(window).height()) {
                $(".icon-back-top").show();
            } else {
                $(".icon-back-top").hide();
            }

            if (!rightFixedEle.parent().hasClass("fr")) {
                return;
            }
            if ($(".blog-main").height() > $(".preview-wrap").height() || $(".blog-main").height() > $(".blog-main>.blog-list").height()) {
                return;
            }
            if (sTop >= top) {
                rightFixedElePH = rightFixedEle.parent().innerHeight() + rightFixedEle.outerHeight(true);
                rightFixedEle.css({
                    "position": "fixed",
                    "top": fixedTop,
                    "left": left
                })
            } else {
                rightFixedElePH = rightFixedEle.parent().innerHeight();
                rightFixedEle.css({
                    "position": "static",
                    "top": fixedTop,
                    "left": left
                })
            }

        }
    }
}

/*添加评论框*/
blogObj.newComment = function (ele, tousername, id) {
    var subCommentEle = ele.parents(".comment-item").children(".sub-comment-list");
    if (subCommentEle.children(".new-comment").length || ele.parents(".comment-item").children(".new-comment").length) {
        subCommentEle.children(".new-comment").children("textarea").attr("placeholder", "回复" + tousername)
        subCommentEle.children(".new-comment").find(".layui-btn").attr("onclick", "blogObj.sendSubComment($(this),'" + tousername + "'," + id + ")")
        return;
    }
    var html = '<div class="new-comment">' +
        '<textarea placeholder="回复' + tousername + '" onfocus="blogObj.textareaFocus($(this),200)" maxlength="200"></textarea>' +
        '<div class="write-function-block clearfix ">' +
        '<div class="fl">您还可以输入<span class="words-num">200</span>个字</div>' +
        '<div class="fr">' +
        '<div class="text-btn mr30" onclick=blogObj.cancelComment($(this).parents(".new-comment"))>取消</div><button class="layui-btn" onclick=blogObj.sendSubComment($(this),"' + tousername + '",' + id + ')>发送</button>' +
        '</div>' +
        '</div>' +
        '</div>';
    if (ele.parents(".user-comment-items").length) { //评论管理页
        if (subCommentEle.length) {
            subCommentEle.after(html);
            blogObj.addHeight(subCommentEle.siblings(".new-comment"), 184);
        } else {
            ele.parents(".comment-item").append(html)
            blogObj.addHeight(ele.parents(".comment-item").children(".new-comment"), 184);
        }
    } else { //博文展示页
        if (subCommentEle.length) {
            subCommentEle.append(html);
            blogObj.addHeight(subCommentEle.children(".new-comment"), 184);
        } else {
            ele.parents(".comment-item").append('<div class="sub-comment-list">' + html +
                '</div>')
            blogObj.addHeight(ele.parents(".comment-item").children(".sub-comment-list").children(".new-comment"), 184);
        }
    }


}

/*取消评论*/
blogObj.cancelComment = function (ele, time) {
    blogObj.addHeight(ele, 0, time);
    var time = time ? time : 300;
    setTimeout(function () {
        ele.remove();
    }, time);
}

/*评论输入框获取光标事件*/
blogObj.textareaFocus = function (ele, maxlength) {
    ele.removeClass("bdc-f6");
    ele.on("input", function () {
        ele.next(".write-function-block").find(".words-num").text(maxlength - ele.val().length);
    });
    if (ele.next().length) {
        return;
    }
    ele.after('' +
        '<div class="write-function-block clearfix">' +
        '<div class="fl">您还可以输入<span class="words-num">200</span>个字</div>' +
        '<div class="fr">' +
        '<div class="text-btn mr30" onclick="blogObj.cancelComment($(this).parent().parent())">取消</div>' +
        '<button class="layui-btn" onclick=blogObj.sendPComment($(this),"/comment/add/")>发送</button>' +
        '</div>' +
        '</div>');
    blogObj.addHeight(ele.next(), 54);
}

