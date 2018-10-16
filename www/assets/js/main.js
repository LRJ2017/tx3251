import Vue from 'vue'
import VueI18n from 'vue-i18n'
Vue.use(VueI18n)

const i18n = new VueI18n({
  locale: localStorage.getItem('lang') || 'cn',    // 语言标识
  messages: {
    'cn': require('./cn'),   // 中文语言包
    'en': require('./en')    // 英文语言包
  }
})
export default i18n